# **********************************************************************
# Core
#
# This all is the the ruby extensions, it's part of the engine.
# **********************************************************************
class LoadsBlocks
  def self.call
    BlockEntry.all.each_with_object({}).with_index { |(block_entry, map), index|
      block_type = index + 1
      map[block_entry.id] = block_type
    }
  end
end

class LoadsBlockTextureAtlas
  TEXTURE_PATH = "./assets/"

  def self.call
    {
      atlas: atlas,
      texture_count: texture_count
    }
  end

  class << self
    private

    def atlas
      @atlas ||=
        BlockEntry.all.each_with_object({}).with_index { |(block_entry, atlas), index|
          block_type = index + 1
          atlas[block_type] ||= {}

          if block_entry.has_texture
            # Every texture is the same, so we only need a single texture index for
            # all sides
            filename = "#{block_entry.id}.png"
            sides = [:north, :south, :east, :west, :top, :bottom]
            atlas[block_type].merge! add_to_atlas(filename, sides)
          elsif block_entry.has_textures.any?
            # Get a texture index for each grouping of sides
            texture_specs = block_entry.has_textures
            texture_specs.each do |texture_spec|
              filename = "#{block_entry.id}-#{texture_spec}.png"
              sides = get_sides_from_spec texture_spec
              atlas[block_type].merge! add_to_atlas(filename, sides)
            end
          else
            raise "No block texture information provided"
          end
        }
    end

    def texture_count
      @texture_count ||=
        begin
          atlas
          @current_texture_index + 1
        end
    end

    def add_to_atlas(filename, sides)
      texture_index = reserve_texture_index
      sides.each_with_object({}) do |side, sides_to_texture_info|
        sides_to_texture_info[side] = {
          texture_index: texture_index,
          path: TEXTURE_PATH + filename
        }
      end
    end

    def get_sides_from_spec(spec)
      case spec
      when :north, :south, :east, :west, :top, :bottom
        [spec]
      else
        spec.to_s.split("").map { |abbreviation|
          {
            "n" => :north,
            "s" => :south,
            "e" => :east,
            "w" => :west,
            "t" => :top,
            "b" => :bottom
          }.fetch(abbreviation)
        }
      end
    end

    def reserve_texture_index
      @current_texture_index ||= -1
      @current_texture_index += 1
      @current_texture_index
    end
  end
end

class ApplicationModel
  def initialize(attributes = {})
    attributes.each do |attribute, value|
      send "#{attribute}=", value
    end
  end
end

class BlockEntry < ApplicationModel
  attr_accessor :id, :name, :has_texture, :has_textures

  # The numeric blockType of a block is the index of the corresponding
  # BlockEntry in this array plus 1.
  @block_entries = []

  def self.create(block_id, &config)
    @block_entries << RegistrationContext.new(block_id).instance_eval(&config)
  end

  def self.all
    @block_entries
  end

  class RegistrationContext
    def initialize(block_id)
      @block_entry = BlockEntry.new id: block_id
    end

    def name(name)
      @block_entry.name = name
      # Return @block_entry at the end of each method so that when
      # instance_eval(&the_block) is called, the return value is @block_entry.
      #
      # I think this little dsl/set of conventions could be codified with some
      # metaprogramming at some point.
      @block_entry
    end

    def has_texture
      @block_entry.has_texture = true
      @block_entry
    end

    def has_textures(*textures)
      @block_entry.has_textures = textures
      @block_entry
    end
  end
end

require "./src/scripts/chunk_generator"

# **********************************************************************
# Load user scripts
# **********************************************************************
Dir.glob("./user_scripts/**/entry.rb").sort.each do |x|
  require x
end
