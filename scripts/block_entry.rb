class BlockEntry
  extend CreatableWithDSL

  attr_accessor :id, :name, :has_texture, :has_textures

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
    @block_entries << context_class.new(BlockEntry.new(id: block_id)).instance_eval(&config)
  end

  def self.all
    @block_entries
  end
end
