# Make and represent the output in ruby, realizing that how it looks in ruby,
# you'll have to write the corresponding mruby C function calls to operate over
# that data, or pull out the relevant values.
#

# **********************************************************************
# Core
#
# This all is the the ruby extensions, it's part of the engine.
# **********************************************************************
class LoadsBlockTextureAtlas
  #
  # Returns {
  #   <blockType> => {
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #   },
  #   <blockType> => {
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #     <side> => <texture index>,
  #   },
  # }
  #
  def self.call
    puts "Loading mrbBlockTextureAtlas into memory..."

    BlockEntry.all.each_with_object({}).with_index do |(block_entry, atlas), index|
      block_type = index + 1
      atlas[block_type] ||= {}

      if block_entry.has_texture
        # Every texture is the same, so we only need a single texture index for
        # all sides
        sides = [:north, :south, :east, :west, :top, :bottom]
        atlas[block_type].merge! add_to_atlas(sides)
      elsif block_entry.has_textures.any?
        # Get a texture index for each grouping of sides
        texture_specs = block_entry.has_textures
        texture_specs.each do |texture_spec|
          sides = get_sides_from_spec texture_spec
          atlas[block_type].merge! add_to_atlas(sides)
        end
      else
        raise "No block texture information provided"
      end

      atlas
    end
  end

  class << self
    private

    # Returns {
    #   :north => <index>,
    #   :south => <index>,
    #   ...
    #   :top => <index>,
    #   :bottom => <index>
    # }
    def add_to_atlas(sides)
      texture_index = reserve_texture_index
      sides.each_with_object({}) do |side, side_to_texture_index|
        side_to_texture_index[side] = texture_index
      end
    end

    def get_sides_from_spec(spec)
      case spec
      when :north, :south, :east, :west, :top, :bottom
        [spec]
      else
        # nsewtb
        spec.to_s.split("").map { |abbreviation|
          case abbreviation
          when "n"
            :north
          when "s"
            :south
          when "e"
            :east
          when "w"
            :west
          when "t"
            :top
          when "b"
            :bottom
          end
        }
      end
    end

    def reserve_texture_index
      @texture_index ||= -1
      @texture_index += 1
      @texture_index
    end
  end
end

class ApplicationModel
  def initialize(attributes = {})
    attributes.each do |attribute|
      public_send "#{attribute}=", attribute
    end
  end
end

class BlockEntry < ApplicationModel
  TEXTURE_PATH = "./assets/"

  attr_accessor :id, :name, :has_texture, :has_textures

  # The numeric blockType of a block is the index of the corresponding
  # BlockEntry in this array plus 1.
  @block_entries = []

  def self.create(block_id, &config)
    @block_entries << RegistrationContext.new.instance_eval(&config)
  end

  def self.all
    @block_entries
  end

  class RegistrationContext
    def initialize
      @block_entry = BlockEntry.new
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
# Userland
#
# This is what a consumer of the api does
# **********************************************************************
BlockEntry.create :bedrock do
  name "Bedrock"
  has_texture
end

BlockEntry.create :dirt do
  name "Dirt"
  has_texture
end

BlockEntry.create :grass do
  name "Grass"
  has_textures :top, :bottom, :nsew
end

BlockEntry.create :cobblestone do
  name "Cobblestone"
  has_texture
end

BlockEntry.create :stone do
  name "Stone"
  has_texture
end
