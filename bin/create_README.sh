#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

# Create directory to store images if it does not already exist.
mkdir -p README

# Delete any previously exported files
if [ ! -n "$(find ./README/ -prune -empty)" ]; then
  rm ./README/*
fi

# Ensure bookstack auth for curl is present

## Assume we are using the default profile
PROFILE="$(grep -E '^\[Profile|^Path|^Default' ~/.mozilla/firefox/profiles.ini | grep -1 '^Default=1' | grep '^Path' | cut -c6-)"

## Create a file to cache session in so we don't have to fetch it every time
## this script runs
##
## If the file is not present or the cookie is expired, refetch (this will
## require closing firefox)
SESSION_CACHE_FILE=.session-cache
generate_session_cache_file() {
  sqlite3 -line "$HOME/.mozilla/firefox/$PROFILE/cookies.sqlite" "SELECT value, expiry FROM moz_cookies WHERE name = 'bookstack_session';" \
  | sed -s 's/ //g' \
  > .session-cache
}
if test -f "$SESSION_CACHE_FILE"; then
  echo "Found .session-cache"
  EXPIRY="$(grep expiry < $SESSION_CACHE_FILE | cut -d'=' -f2)"
  NOW="$(date +%s)"
  if [ "$EXPIRY" -le "$NOW" ]; then
    echo "Cookie is expired, attempting to fetch new one"
    generate_session_cache_file
  else
    echo "Cookie is valid"
  fi
else
  echo "Missing .session-cache. Attempting to fetch new one"
  generate_session_cache_file
fi

# Fetch the post
# In order to read from the cookie database, firefox will have to be closed
# (this will also ensure current value is flushed to db)
SESSION="$(grep value < .session-cache | cut -d'=' -f2)"
curl 'http://localhost:8080/books/projects/page/creating-a-voxel-engine-from-scratch/export/html' \
  --silent \
  -H "Cookie: bookstack_session=$SESSION" \
  > export.html

# Extract the images
IMAGE_REGEX='(http://localhost:8080)?/uploads/images/gallery/\d{4}-\d{2}-\w{3}/[a-zA-Z0-9_\-\.]+'
DATA_REGEX='data:image/[^;]*;base64,\K[a-zA-Z0-9+/=]*'
grep --color=never --perl-regexp "$IMAGE_REGEX" export.html | while IFS= read -r line
do
  # Create a tempfile for our matched line
  FILE="$(mktemp)"

  # Write matching line to file
  echo "$line" > "$FILE"

  # Extract base64 image data and image url
  BASE64_DATA="$(grep --color=never --perl-regexp --only-matching "$DATA_REGEX" "$FILE")"
  IMAGE_URL="$(grep --color=never --perl-regexp --only-matching "$IMAGE_REGEX" "$FILE")"
  echo "Found image $IMAGE_URL"

  # Cleanup tempfile
  rm "$FILE"

  # Craft local image path
  README_FILE="./README/$(basename "$IMAGE_URL")"

  # Decode base64 data and write to image file
  echo -n "$BASE64_DATA" | base64 --decode --ignore-garbage > "$README_FILE"

  # Replace url with relative link to new file
  sed -i -e "s|$IMAGE_URL|$README_FILE|g" export.html

  # Replace img tag src with path to new file
  IMG_SRC_REGEX='data:image/[^;]*;base64,[a-zA-Z0-9+/=]*'
  sed -i -e "s|$IMG_SRC_REGEX|$README_FILE|g" export.html
done

# Convert html file to github markdown
# Remove the automatically included final section which contains metadata
reverse_markdown export.html --github-flavored=true | tac | sed '0,/* * */d' | tac > README.md

rm export.html
