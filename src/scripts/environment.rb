# **********************************************************************
# Load Engine scripts
# **********************************************************************
# Shared
require "./src/scripts/creatable_with_dsl"

require "./src/scripts/block_entry"

# Used in: Texture as static class member
require "./src/scripts/loads_block_texture_atlas"

# Used in: Chunk#generate
require "./src/scripts/generates_chunks"
require "./src/scripts/loads_blocks"

# **********************************************************************
# Load User scripts (i.e. mods)
# **********************************************************************
Dir.glob("./user_scripts/**/entry.rb").sort.each do |entry_file|
  require entry_file
end
