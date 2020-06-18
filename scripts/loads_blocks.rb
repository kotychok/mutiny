class LoadsBlocks
  def self.call
    BlockEntry.all.each_with_object({}).with_index { |(block_entry, map), index|
      block_type = index + 1
      map[block_entry.id] = block_type
    }
  end
end
