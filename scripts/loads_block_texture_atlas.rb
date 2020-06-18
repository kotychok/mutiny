class LoadsBlockTextureAtlas
  def self.call
    {
      atlas: atlas,
      texture_count: texture_count
    }
  end

  class << self
    private

    # "Block Texture Atlas"
    #
    #     {
    #       <int corresponding to Bedrock block type> => {
    #         north: {
    #           texture_index: 0,
    #           path: "./assets/bedrock.png",
    #         },
    #         ...
    #         bottom: {
    #           texture_index: 0,
    #           path: "./assets/bedrock.png",
    #         },
    #       },
    #       ...
    #       <int corresponding to Grass block type> => {
    #         north: {
    #           texture_index: 1,
    #           path: "./assets/grass-nsew.png",
    #         },
    #         ...
    #         bottom: {
    #           texture_index: 3,
    #           path: "./assets/grass-bottom.png",
    #         },
    #       },
    #       ...
    #     }

    def atlas
      @atlas ||=
        begin
          atlas = {}
          texture_index = 0

          block_entries = BlockEntry.all

          block_entries.each do |block_entry|
            texture_path_to_sides = block_entry.textures
            atlas[block_entry.block_type] ||= {}

            texture_path_to_sides.each do |texture_path, sides|
              atlas[block_entry.block_type].merge!(
                sides.each_with_object({}) { |side, side_data|
                  side_data[side] = {
                    texture_index: texture_index,
                    path: texture_path
                  }
                }
              )

              texture_index += 1
            end
          end

          atlas
        end
    end

    def texture_count
      @texture_count ||=
        begin
          atlas
            .values
            .flat_map(&:values)
            .map { |h| h[:texture_index] }
            .max + 1
        end
    end
  end
end
