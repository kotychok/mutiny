#include <array>
#include <algorithm>
#include <cmath>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>

#include "all_equal.h"
#include "chunk.h"

std::vector<quad> chunkToQuads(const Chunk &chunk) {
  std::vector<quad> quads {};

  // Sweep over each axis (X (d = 0), Y (d = 1) and Z (d = 2))
  for (auto d = 0; d < 3; ++d)
  {
    int i, j, k, l, w, h;
    int u = (d + 1) % 3;
    int v = (d + 2) % 3;
    float x[3] {};
    float q[3] {};

    bool mask[Chunk::SIZE * Chunk::SIZE] {};
    q[d] = 1;

    // Check each slice of the chunk one at a time
    for (x[d] = -1; x[d] < Chunk::SIZE;)
    {
      // Compute the mask
      auto n = 0;
      for (x[v] = 0; x[v] < Chunk::SIZE; ++x[v])
      {
        for (x[u] = 0; x[u] < Chunk::SIZE; ++x[u])
        {
          // q determines the direction (X, Y or Z) that we are searching
          // chunk.isBlockAt(x,y,z) takes local-to-chunk map positions and returns true if a block exists there

          bool blockCurrent = 0 <= x[d]              ? chunk.isBlockAt(x[0],        x[1],        x[2])        : false;
          bool blockCompare = x[d] < Chunk::SIZE - 1 ? chunk.isBlockAt(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : false;

          // The mask is set to true if there is a visible face between two blocks,
          //   i.e. both aren't empty and both aren't blocks
          mask[n++] = blockCurrent != blockCompare;
        }
      }

      ++x[d];

      n = 0;

      // Generate a mesh from the mask using lexicographic ordering,
      //   by looping over each block in this slice of the chunk
      for (j = 0; j < Chunk::SIZE; ++j)
      {
        for (i = 0; i < Chunk::SIZE;)
        {
          if (mask[n])
          {
            // Compute the width of this quad and store it in w
            //   This is done by searching along the current axis until mask[n + w] is false
            for (w = 1; i + w < Chunk::SIZE && mask[n + w]; w++) { }

            // Compute the height of this quad and store it in h
            //   This is done by checking if every block next to this row (range 0 to w) is also part of the mask.
            //   For example, if w is 5 we currently have a quad of dimensions 1 x 5. To reduce triangle count,
            //   greedy meshing will attempt to expand this quad out to Chunk::SIZE x 5, but will stop if it reaches a hole in the mask

            auto done = false;
            for (h = 1; j + h < Chunk::SIZE; h++)
            {
              // Check each block next to this quad
              for (k = 0; k < w; ++k)
              {
                // If there's a hole in the mask, exit
                if (!mask[n + k + h * Chunk::SIZE])
                {
                  done = true;
                  break;
                }
              }

              if (done)
                break;
            }

            x[u] = i;
            x[v] = j;

            // du and dv determine the size and orientation of this face
            float du[3] {};
            du[u] = w;

            float dv[3] {};
            dv[v] = h;

            // Create a quad for this face. Colour, normal or textures are not stored in this block vertex format.
            quad quad {
              x[0]                 - Chunk::SIZE / 2, x[1]                 - 1, x[2]                 - Chunk::SIZE / 2, // Top-left vertice position
              x[0] + du[0]         - Chunk::SIZE / 2, x[1] + du[1]         - 1, x[2] + du[2]         - Chunk::SIZE / 2, // Top right vertice position
              x[0] + dv[0]         - Chunk::SIZE / 2, x[1] + dv[1]         - 1, x[2] + dv[2]         - Chunk::SIZE / 2, // Bottom left vertice position
              x[0] + du[0] + dv[0] - Chunk::SIZE / 2, x[1] + du[1] + dv[1] - 1, x[2] + du[2] + dv[2] - Chunk::SIZE / 2  // Bottom right vertice position
            };

            quads.push_back(quad);

            // Clear this part of the mask, so we don't add duplicate faces
            for (l = 0; l < h; ++l)
              for (k = 0; k < w; ++k)
                mask[n + k + l * Chunk::SIZE] = false;

            // Increment counters and continue
            i += w;
            n += w;
          }
          else
          {
            i++;
            n++;
          }
        }
      }
    }
  }

  return quads;
}

quad_mesh quadToQuadMesh(const quad &quad) {
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

  return quadMesh;
}

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
  std::cout << "Chunk " << this << " at " << pos.x << ", " << pos.y << ", " << pos.z << " created" << std::endl;

  // Cubes
  glGenVertexArrays(1, &chunkVAO);
  glGenBuffers(1, &chunkVBO);

  if (pos.y == 0) {
    for (unsigned int x = 0; x < Chunk::SIZE; x++) {
      for (unsigned int z = 0; z < Chunk::SIZE; z++) {
        unsigned int index { z * SIZE * SIZE + 0 * SIZE + x };
        blocks[index] = 1;
      }
    }

    // int radius = 8;
    // for (unsigned int x = 0; x < Chunk::SIZE; x++) {
      // for (unsigned int z = 0; z < Chunk::SIZE; z++) {
        // for (unsigned int y = 0; y < Chunk::SIZE; y++) {
          // if (y < 8) {
            // unsigned int blockX = x - Chunk::SIZE / 2;
            // unsigned int blockY = y - Chunk::SIZE / 2;
            // unsigned int blockZ = z - Chunk::SIZE / 2;
            // if (sqrt(blockX * blockX + blockY * blockY + blockZ * blockZ) < radius) {
              // unsigned int index { z * SIZE * SIZE + y * SIZE + x };
              // blocks[index] = 1;
            // }
          // }
        // }
      // }
    // }
  }

  quads = chunkToQuads(*this);
  transform(quads.begin(), quads.end(), back_inserter(quadMeshes), quadToQuadMesh);
}

Chunk::~Chunk() {
  std::cout << "Chunk " << this << " at " << pos.x << ", " << pos.y << ", " << pos.z << " destroyed" << std::endl;
}

bool Chunk::isBlockAt(int x, int y, int z) const {
  int index { z * SIZE * SIZE + y * SIZE + x };
  return blocks[index] == 1;
}

void Chunk::render(const Shader &myShader) {
  for (quad_mesh quadMesh : quadMeshes) {
    glBindVertexArray(chunkVAO);

    glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadMesh), quadMesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::mat4 blockModel = glm::mat4(1.0);
    blockModel = glm::translate(blockModel, pos * Chunk::SIZE);
    myShader.setMat4("model", blockModel);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
