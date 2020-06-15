class ChunkGenerator
  def self.flat(pos_x, pos_y, pos_z)
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

  def self.flat_half_and_half(pos_x, pos_y, pos_z)
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

  def self.flat_half_and_half_with_square(pos_x, pos_y, pos_z)
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

  def self.flat_random(pos_x, pos_y, pos_z)
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

  def self.flat_with_plus(pos_x, pos_y, pos_z)
    return [] if pos_y != 0

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

  def self.half(pos_x, pos_y, pos_z)
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

  def self.filled(pos_x, pos_y, pos_z, block_id = :dirt)
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

  def self.half_sphere(pos_x, pos_y, pos_z)
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

  def self.perlin(pos_x, pos_y, pos_z)
    blocks = []
    case pos_y
    when 0..1
      blocks = filled pos_x, pos_y, pos_z, :stone
    when 2
      min_height = 0
      max_height = CHUNK_SIZE

      CHUNK_SIZE.times do |block_x|
        CHUNK_SIZE.times do |block_z|
          noise_x = block_x / CHUNK_SIZE + pos_x
          noise_z = block_z / CHUNK_SIZE + pos_z

          generated_noise = (1 + CNoise.get_perlin_noise(noise_x, 0.0, noise_z)) / 2.0
          noise = generated_noise.clamp(0.0, 1.0)
          raw_height = min_height + noise * (max_height + 1 - min_height)
          height = raw_height.clamp(min_height, max_height)

          height.times do |y|
            index = block_z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + block_x
            blocks[index] =
              if y <= height - 3
                :stone
              else
                :grass
              end
          end
        end
      end
    end
    blocks
  end
end
