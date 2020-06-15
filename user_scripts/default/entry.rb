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
