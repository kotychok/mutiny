class GeneratesChunks
  class Shape
    def self.plus(x, y, z, size: 1, filled: false)
      v = [
        [x + size, y, z],
        [x - size, y, z],
        [x, y, z + size],
        [x, y, z - size]
      ]
      v << [x, y, z] if filled
      return v if size == 1
      [*v, *plus(x, y, z, size: size - 1)]
    end

    def self.circle(x, y, z, filled: false)
      [
        *plus(x, y, z, size: 1, filled: filled),
        [x + 1, y, z + 1],
        [x + 1, y, z - 1],
        [x - 1, y, z + 1],
        [x - 1, y, z - 1]
      ]
    end

    def self.vertical(x, y, z, height: 1)
      height.times.map { |h|
        [x, y + h, z]
      }
    end
  end

  class WorldGen
    def self.simple_tree(blocks, x, y, z)
      # Get where our logs should go.
      trunk_coords = Shape.vertical(x, y, z, height: 4)

      # Return false if trunk would end up outside chunk or a spot needed is
      # already taken.
      return false if trunk_coords.any? { |x, y, z|
        is_outside(x, y, z) || !blocks[index3d(x, y, z)].nil?
      }

      # Same for leaves, get where our leaves should go.
      leaves_coords = [
        *Shape.plus(x, y + 3, z, size: 2),
        *Shape.circle(x, y + 3, z),
        *Shape.plus(x, y + 4, z, filled: true),
        [x, (y + 5), z]
      ]

      # Similarly, return false is leaves would end up outside chunk or a
      # spot needed is already taken.
      return false if leaves_coords.any? { |x, y, z|
        is_outside(x, y, z) || !blocks[index3d(x, y, z)].nil?
      }

      # Make our tree and return new blocks
      trunk_coords.each do |x, y, z|
        blocks[index3d(x, y, z)] = :oak
      end

      leaves_coords.each do |x, y, z|
        blocks[index3d(x, y, z)] = :leaves
      end

      blocks
    end

    def self.is_outside(x, y, z)
      x < 0 || x > CHUNK_SIZE - 1 ||
        y < 0 || y > CHUNK_SIZE - 1 ||
        z < 0 || z > CHUNK_SIZE - 1
    end

    def self.index3d(x, y, z)
      x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED
    end
  end

  def self.flat(chunk_x, chunk_y, chunk_z)
    y = 0
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
        blocks[index] = :dirt
      end
    end
    blocks
  end

  def self.flat_half_and_half(chunk_x, chunk_y, chunk_z)
    y = 0
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
        blocks[index] =
          if x < CHUNK_SIZE_HALVED
            :dirt
          else
            :stone
          end
      end
    end
    blocks
  end

  def self.flat_half_and_half_with_square(chunk_x, chunk_y, chunk_z)
    y = 0
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
        blocks[index] =
          if x > CHUNK_SIZE / 4 && x < 3 * CHUNK_SIZE / 4 &&
              z > CHUNK_SIZE / 4 && z < 3 * CHUNK_SIZE / 4

            :bedrock
          elsif x < CHUNK_SIZE_HALVED
            :dirt
          else
            :stone
          end
      end
    end
    blocks
  end

  def self.flat_random(chunk_x, chunk_y, chunk_z)
    y = 0
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
        blocks[index] = if rand < 0.5
          :dirt
        else
          :stone
        end
      end
    end
    blocks
  end

  def self.flat_with_plus(chunk_x, chunk_y, chunk_z)
    return [] if chunk_y != 0

    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        CHUNK_SIZE_QUARTERED.times do |y|
          index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
          blocks[index] =
            if y.zero?
              :bedrock
            elsif x == CHUNK_SIZE_HALVED || z == CHUNK_SIZE_HALVED
              :dirt
            end
        end
      end
    end
    blocks
  end

  def self.half(chunk_x, chunk_y, chunk_z)
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        CHUNK_SIZE_HALVED.times do |y|
          index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
          blocks[index] = :dirt
        end
      end
    end
    blocks
  end

  def self.filled(chunk_x, chunk_y, chunk_z, block_id = :dirt)
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        CHUNK_SIZE.times do |y|
          index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
          blocks[index] = block_id
        end
      end
    end
    blocks
  end

  def self.half_sphere(chunk_x, chunk_y, chunk_z)
    blocks = []
    radius = CHUNK_SIZE_HALVED
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        8.times do |y|
          block_x = x - CHUNK_SIZE / 2
          block_y = y - CHUNK_SIZE / 2
          block_z = z - CHUNK_SIZE / 2
          if Math.sqrt(block_x * block_x + block_y * block_y + block_z * block_z) < radius
            index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
            blocks[index] = :dirt
          end
        end
      end
    end
    blocks
  end

  def self.perlin(chunk_x, chunk_y, chunk_z)
    blocks = []

    case chunk_y
    when 0..1
      blocks = filled chunk_x, chunk_y, chunk_z, :stone
    when 2
      heights = []
      max_height = CHUNK_SIZE
      min_height = 0

      # Generate landscape
      CHUNK_SIZE.times do |block_x|
        CHUNK_SIZE.times do |block_z|
          noise_x = block_x / CHUNK_SIZE + chunk_x
          noise_z = block_z / CHUNK_SIZE + chunk_z

          generated_noise = (1 + CNoise.perlin(noise_x, 0.0, noise_z)) / 2.0
          noise = generated_noise.clamp(0.0, 1.0)
          raw_height = min_height + noise * (max_height + 1 - min_height)
          height = raw_height.clamp(min_height, max_height).floor
          heights[block_z * CHUNK_SIZE + block_x] = height

          height.times do |y|
            index = block_z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + block_x
            blocks[index] =
              if y <= height - 3
                :stone
              elsif y <= height - 2
                :dirt
              else
                :grass
              end
          end
        end
      end

      # Add trees
      trees_per_chunk = 3
      trees_per_chunk.times do
        x = (3..(CHUNK_SIZE - 4)).to_a.sample
        z = (3..(CHUNK_SIZE - 4)).to_a.sample
        surface_y = heights[z * CHUNK_SIZE + x]
        if (result = WorldGen.simple_tree(blocks, x, surface_y, z))
          blocks = result
        end
      end
    end
    blocks
  end
end
