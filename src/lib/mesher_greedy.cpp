// https://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
// https://github.com/mikolalysenko/mikolalysenko.github.com/blob/gh-pages/MinecraftMeshes/js/greedy.js
// https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33

#include "all_equal.h"
#include "mesher_greedy.h"

std::vector<std::pair<quad, BlockType>> MesherGreedy::chunkToQuads(const Chunk &chunk) {
  std::vector<std::pair<quad, BlockType>> quads {};

  // Sweep over each axis (X (d = 0), Y (d = 1) and Z (d = 2))
  for (auto sweepAxis = 0; sweepAxis < 3; ++sweepAxis) {
    int col, row, k, l, width, height;

    int orthoAxisU = (sweepAxis + 1) % 3;
    int orthoAxisV = (sweepAxis + 2) % 3;
    float currBlockCoords[3] {};

    bool mask[CHUNK_SIZE_SQUARED] {};

    // q determines the direction (X, Y or Z) that we are searching
    float q[3] {};
    q[sweepAxis] = 1;

    // Check each slice of the chunk one at a time
    for (currBlockCoords[sweepAxis] = -1; currBlockCoords[sweepAxis] < CHUNK_SIZE;) {
      // Compute the mask
      auto n = 0;
      for (currBlockCoords[orthoAxisV] = 0; currBlockCoords[orthoAxisV] < CHUNK_SIZE; ++currBlockCoords[orthoAxisV]) {
        for (currBlockCoords[orthoAxisU] = 0; currBlockCoords[orthoAxisU] < CHUNK_SIZE; ++currBlockCoords[orthoAxisU]) {
          // chunk.isBlockAt(x,y,z) takes local-to-chunk map positions and returns true if a block exists there

          bool blockCurrent = 0 <= currBlockCoords[sweepAxis]             ? chunk.isBlockAt(currBlockCoords[0],        currBlockCoords[1],        currBlockCoords[2])        : false;
          bool blockCompare = currBlockCoords[sweepAxis] < CHUNK_SIZE - 1 ? chunk.isBlockAt(currBlockCoords[0] + q[0], currBlockCoords[1] + q[1], currBlockCoords[2] + q[2]) : false;

          // The mask is set to true if there is a visible face between two blocks,
          //   i.e. both aren't empty and both aren't blocks
          mask[n++] = blockCurrent != blockCompare;
        }
      }

      ++currBlockCoords[sweepAxis];

      n = 0;

      // Generate a mesh from the mask using lexicographic ordering,
      //   by looping over each block in this slice of the chunk
      for (row = 0; row < CHUNK_SIZE; ++row) {
        for (col = 0; col < CHUNK_SIZE;) {
          if (mask[n]) {
            // Compute the width of this quad and store it in width
            //   This is done by searching along the current axis until mask[n + width] is false
            for (width = 1; col + width < CHUNK_SIZE && mask[n + width]; width++) { }

            // Compute the height of this quad and store it in height
            //   This is done by checking if every block next to this row (range 0 to width) is also part of the mask.
            //   For example, if width is 5 we currently have a quad of dimensions 1 x 5. To reduce triangle count,
            //   greedy meshing will attempt to expand this quad out to CHUNK_SIZE x 5, but will stop if it reaches a hole in the mask

            auto done = false;
            for (height = 1; row + height < CHUNK_SIZE; height++) {
              // Check each block next to this quad
              for (k = 0; k < width; ++k) {
                // If there's a hole in the mask, exit
                if (!mask[n + k + height * CHUNK_SIZE]) {
                  done = true;
                  break;
                }
              }

              if (done) {
                break;
              }
            }

            currBlockCoords[orthoAxisU] = col;
            currBlockCoords[orthoAxisV] = row;

            // du and dv determine the size and orientation of this face
            float du[3] {};
            du[orthoAxisU] = width;

            float dv[3] {};
            dv[orthoAxisV] = height;

            // Create a quad for this face. Colour, normal or textures are not stored in this block vertex format.
            quad quad {
              currBlockCoords[0]                 - CHUNK_SIZE_HALVED, currBlockCoords[1]                 - 1, currBlockCoords[2]                 - CHUNK_SIZE_HALVED, // Top-left vertice position
              currBlockCoords[0] + du[0]         - CHUNK_SIZE_HALVED, currBlockCoords[1] + du[1]         - 1, currBlockCoords[2] + du[2]         - CHUNK_SIZE_HALVED, // Top right vertice position
              currBlockCoords[0] + dv[0]         - CHUNK_SIZE_HALVED, currBlockCoords[1] + dv[1]         - 1, currBlockCoords[2] + dv[2]         - CHUNK_SIZE_HALVED, // Bottom left vertice position
              currBlockCoords[0] + du[0] + dv[0] - CHUNK_SIZE_HALVED, currBlockCoords[1] + du[1] + dv[1] - 1, currBlockCoords[2] + du[2] + dv[2] - CHUNK_SIZE_HALVED  // Bottom right vertice position
            };

            quads.push_back(std::make_pair(quad, BlockType::DIRT));

            // Clear this part of the mask, so we don't add duplicate faces
            for (l = 0; l < height; ++l) {
              for (k = 0; k < width; ++k) {
                mask[n + k + l * CHUNK_SIZE] = false;
              }
            }

            // Increment counters and continue
            col += width;
            n += width;
          } else {
            col++;
            n++;
          }
        }
      }
    }
  }

  return quads;
}

std::pair<quad_mesh, BlockType> MesherGreedy::quadToQuadMesh(const std::pair<quad, BlockType> &pair) {
  const quad& quad { pair.first };
  const BlockType& blockType { pair.second };

  float rb_x { quad.at(9) }, rb_y { quad.at(10) }, rb_z { quad.at(11) }, // right bottom
        rt_x { quad.at(3) }, rt_y { quad.at(4) },  rt_z { quad.at(5) },  // right top
        lb_x { quad.at(6) }, lb_y { quad.at(7) },  lb_z { quad.at(8) },  // left bottom
        lt_x { quad.at(0) }, lt_y { quad.at(1) },  lt_z { quad.at(2) };  // left top

  float rb_u, rb_v, // right bottom
        rt_u, rt_v, // right top
        lb_u, lb_v, // left bottom
        lt_u, lt_v; // left top

  if (all_equal(rb_x, rt_x, lb_x, lt_x)) {
    // If x is constant,
    // then left-to-right (u-axis) is along the z-axis
    // and top-to-bottom (v-axis) is along the y-axis.
    rb_u = rb_z;
    rt_u = rt_z;
    lb_u = lb_z;
    lt_u = lt_z;

    rb_v = rb_y;
    rt_v = rt_y;
    lb_v = lb_y;
    lt_v = lt_y;
  } else if (all_equal(rb_y, rt_y, lb_y, lt_y)) {
    // If y is constant,
    // then left-to-right (u-axis) is along the x-axis
    // and top-to-bottom (v-axis) is along the z-axis.
    rb_u = rb_x;
    rt_u = rt_x;
    lb_u = lb_x;
    lt_u = lt_x;

    rb_v = rb_z;
    rt_v = rt_z;
    lb_v = lb_z;
    lt_v = lt_z;
  } else if (all_equal(rb_z, rt_z, lb_z, lt_z)) {
    // If z is constant,
    // then left-to-right (u-axis) is along the x-axis
    // and top-to-bottom (v-axis) is along the y-axis.
    rb_u = rb_x;
    rt_u = rt_x;
    lb_u = lb_x;
    lt_u = lt_x;

    rb_v = rb_y;
    rt_v = rt_y;
    lb_v = lb_y;
    lt_v = lt_y;
  }

  quad_mesh quadMesh {
    rb_x, rb_y, rb_z, rb_u, rb_v, // right bottom
    rt_x, rt_y, rt_z, rt_u, rt_v, // right top
    lb_x, lb_y, lb_z, lb_u, lb_v, // left bottom

    lt_x, lt_y, lt_z, lt_u, lt_v, // left top
    rt_x, rt_y, rt_z, rt_u, rt_v, // right top
    lb_x, lb_y, lb_z, lb_u, lb_v, // left bottom
  };

  return std::make_pair(quadMesh, blockType);
}
