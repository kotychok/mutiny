#include <array>
#include <algorithm>
#include <cmath>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>

#include "all_equal.h"
#include "chunk.h"

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

  if (pos.y > 0) {
    return;
  }
  for (unsigned int x = 0; x < Chunk::SIZE; x++) {
    for (unsigned int z = 0; z < Chunk::SIZE; z++) {
      unsigned int index { z * SIZE * SIZE + 0 * SIZE + x };
      blocks[index] = 1;
    }
  }

  transform(quads.begin(), quads.end(), back_inserter(quadMeshes), quadToQuadMesh);
}

Chunk::~Chunk() {
  std::cout << "Chunk " << this << " at " << pos.x << ", " << pos.y << ", " << pos.z << " destroyed" << std::endl;
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
