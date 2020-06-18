class BlockEntry
  extend CreatableWithDSL

  attr_accessor :id, :pack, :block_type, :name, :has_texture, :has_textures

  dsl :name, :string
  dsl :has_texture, :boolean
  dsl :has_textures, :array

  def initialize(attributes = {})
    attributes.each do |attribute, value|
      send "#{attribute}=", value
    end
  end

  # The numeric blockType of a block is the index of the corresponding
  # BlockEntry in this array plus 1.
  @block_entries = []

  def self.create(block_id, &config)
    @block_entries << context_class.new(
      BlockEntry.new(
        id: block_id,
        block_type: @block_entries.size + 1
      )
    ).instance_eval(&config)
  end

  def self.all
    @block_entries
  end

  def textures
    if has_texture
      # Every texture is the same, so we only need a single texture index for
      # all sides
      texture_path = "./user_scripts/#{pack}/assets/#{id}.png"
      sides = [:north, :south, :east, :west, :top, :bottom]
      {texture_path => sides}
    elsif has_textures.any?
      # Get a texture index for each grouping of sides
      has_textures.each_with_object({}) do |texture_spec, h|
        texture_path = "./user_scripts/#{pack}/assets/#{id}-#{texture_spec}.png"
        sides = get_sides_from_spec texture_spec
        h[texture_path] = sides
      end
    else
      raise "No block texture information provided"
    end
  end

  private

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
end
