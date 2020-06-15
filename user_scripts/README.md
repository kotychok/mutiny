# user_scripts directory

I'm going to define a convention where everything else in this directory is
itself a directory to a "mod" -- a bundle of ruby scripts.

There needs to be at least one file in that mod directory, an `entry.rb` file
which is the one file that will be required per mod. The author of the mod will
have to require any other files if desired.
