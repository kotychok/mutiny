class LoadsBlockTextureAtlas
  TEXTURE_PATH = "./user_scripts/"

  def self.call
    {
      atlas: atlas,
      texture_count: texture_count
    }
  end

  class << self
    private

    def atlas
      @atlas ||=
        BlockEntry.all.each_with_object({}).with_index { |(block_entry, atlas), index|
          block_type = index + 1
          atlas[block_type] ||= {}

          if block_entry.has_texture
            # Every texture is the same, so we only need a single texture index for
            # all sides
            filename = "#{block_entry.pack}/assets/#{block_entry.id}.png"
            sides = [:north, :south, :east, :west, :top, :bottom]
            atlas[block_type].merge! add_to_atlas(filename, sides)
          elsif block_entry.has_textures.any?
            # Get a texture index for each grouping of sides
            texture_specs = block_entry.has_textures
            texture_specs.each do |texture_spec|
              filename = "#{block_entry.pack}/assets/#{block_entry.id}-#{texture_spec}.png"
              sides = get_sides_from_spec texture_spec
              atlas[block_type].merge! add_to_atlas(filename, sides)
            end
          else
            raise "No block texture information provided"
          end
        }
    end

    def texture_count
      @texture_count ||=
        begin
          atlas
          @current_texture_index + 1
        end
    end

    def add_to_atlas(filename, sides)
      path = TEXTURE_PATH + filename
      texture_index = reserve_texture_index
      sides.each_with_object({}) do |side, sides_to_texture_info|
        sides_to_texture_info[side] = {
          texture_index: texture_index,
          path: path
        }
      end
    end

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

    def reserve_texture_index
      @current_texture_index ||= -1
      @current_texture_index += 1
      @current_texture_index
    end
  end
end
