# **********************************************************************
# Load Engine scripts
# **********************************************************************
# Shared
require "./scripts/creatable_with_dsl"

require "./scripts/block_entry"

# Used in: Texture as static class member
require "./scripts/loads_block_texture_atlas"

# Used in: Chunk#generate
require "./scripts/generates_chunks"
require "./scripts/loads_blocks"

# **********************************************************************
# Load User scripts (i.e. mods)
# **********************************************************************
Dir.glob("./user_scripts/**/entry.rb").sort.each do |entry_file|
  require entry_file
end
