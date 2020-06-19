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

BlockEntry.create :oak do
  name "Oak"
  has_textures :nsew, :tb
end

BlockEntry.create :leaves do
  name "Leaves"
  has_texture
end

BlockEntry.create :mossy_stone_brick do
  name "Mossy Stone Brick"
  has_texture
end
