puts 'Hello, World from a file!'

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

class ChunkGenerator
  def self.flat
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
end
