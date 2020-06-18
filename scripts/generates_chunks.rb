class GeneratesChunks
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
      # Generate landscape
      min_height = 0
      max_height = CHUNK_SIZE

      CHUNK_SIZE.times do |block_x|
        CHUNK_SIZE.times do |block_z|
          noise_x = block_x / CHUNK_SIZE + chunk_x
          noise_z = block_z / CHUNK_SIZE + chunk_z

          generated_noise = (1 + CNoise.perlin(noise_x, 0.0, noise_z)) / 2.0
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
