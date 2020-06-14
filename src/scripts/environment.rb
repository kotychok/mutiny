# At the top level, I am somewhat confident the API I want is something like
# this.
#
# A single method that gets called, and will likely call other methods to fill
# in parts of the world.
#
# I still am unsure whether to treat land masses as different from structures.
#
# Since I don't know what I don't know, perhaps the best way to start is by
# doing the absolute most simple thing, and seeing if that is sufficient.
#
# I am unsure... but I feel like "put this block there" is about as simple as
# it gets.
#
# I am unsure how to handle/abstract away chunk boundaries.
#
# Perhaps the simplest thing would be to not abstract away anything that
# doesn't need to be now. I might not get the nicest code, but it's the
# simplest/least amount I need for now. And since none of my current world gen
# is cross-chunk anyways, it's a moot point.


# This is the simplest. A direct port of the C++ to the Ruby, _except_ I return
# an array of strings, not an array of structs like the C++ does. It seemed
# easier to me to return strings and do the struct creation in C++ for now.
#
# It seems plausible that if I can in ruby represent this struct and return it
# and have that understood correctly by the C++ part then that would be better,
# but idk if that's possible. This might be a defining part of my api. Perhaps
# I will often reference things by some string id, e.g. "dirt", "some_item",
# and pass that into C++ where it is then used to create the thing in memory
# (could also send in extra data as well to the C++, options, configuration,
# starting values and such).
#
# Maybe intentionally converting only simple types across the C++ and Ruby
# layer is even good for simplicity's sake? But this might limit the
# expressiveness of the ruby API and make it less enjoyable to use? Will have
# to investigate this. So first, let's stick with the "only simple types" thing
# for now, and see how far it gets us
#
# However, before we can do any of that, we need to get C++ calling this code
# before we can start caring about the return value or what it does.
#
# Let' see... can I print out a reference to the function from C++? Let's see
# if mruby has an Object#method method.
#
# A successful test would be calling this method from C++ and printing out the
# first value in the array (i.e. "dirt")

# TODO Constants should be defined from C++ where they are defined in C++
# instead of copy-pasted.
CHUNK_SIZE = 32;
CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
CHUNK_SIZE_HALVED = CHUNK_SIZE / 2;
CHUNK_SIZE_QUARTERED = CHUNK_SIZE / 4;

class ChunkGenerator
  def self.flat(pos_x, pos_y, pos_z)
    y = 0
    blocks = []
    CHUNK_SIZE.times do |x|
      CHUNK_SIZE.times do |z|
        index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
        blocks[index] = "dirt"
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
            "dirt"
          else
            "stone"
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

        "bedrock"
          elsif x < CHUNK_SIZE_HALVED
            "dirt"
          else
            "stone"
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
        if rand() < 0.5
          blocks[index] = "dirt"
        else
          blocks[index] = "stone"
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
              "bedrock"
            elsif x == CHUNK_SIZE_HALVED || z == CHUNK_SIZE_HALVED
              "dirt"
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
          blocks[index] = "dirt"
        end
      end
    end
    blocks
  end

  def self.filled(pos_x, pos_y, pos_z, block_id = "dirt")
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
          block_x = x - CHUNK_SIZE / 2;
          blockY = y - CHUNK_SIZE / 2;
          block_z = z - CHUNK_SIZE / 2;
          if (Math.sqrt(block_x * block_x + blockY * blockY + block_z * block_z) < radius)
            index = z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x
            blocks[index] = "dirt"
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
      blocks = filled pos_x, pos_y, pos_z, "stone"
    when 2
      min_height = 0
      max_height = CHUNK_SIZE

      CHUNK_SIZE.times do |block_x|
        CHUNK_SIZE.times do |block_z|
          noise_x = block_x / CHUNK_SIZE + pos_x
          noise_z = block_z / CHUNK_SIZE + pos_z

          generated_noise = (1 + PerlinNoise.get_value(noise_x, 0.0, noise_z)) / 2.0
          noise = generated_noise.clamp(0.0, 1.0)
          raw_height = min_height + noise * (max_height + 1 - min_height)
          height = raw_height.clamp(min_height, max_height)

          height.times do |y|
            index = block_z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + block_x
            blocks[index] =
              if y <= height - 3
                "stone"
              else
                "grass"
              end
          end
        end
      end
    end
    return blocks
  end
end
