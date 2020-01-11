#pragma once

#include <array>
#include <vector>

#include "shader.h"

// This contains the 4 vertices (x, y, z) for a quad.
// The vertices are given in the following order:
//
// float
//   lt_x { quad.at(0) }, lt_y { quad.at(1) },  lt_z { quad.at(2) };  // left top
//   rt_x { quad.at(3) }, rt_y { quad.at(4) },  rt_z { quad.at(5) },  // right top
//   lb_x { quad.at(6) }, lb_y { quad.at(7) },  lb_z { quad.at(8) },  // left bottom
//   rb_x { quad.at(9) }, rb_y { quad.at(10) }, rb_z { quad.at(11) }, // right bottom
typedef std::array<float, 12> quad;

// This contains all the vertices needed to render the quad, which is composed
// of 2 triangles. It also contains the texture coordinates.
//
// The order of the vertices used to compose each triangle can be seen as
// follows:
//
// quad_mesh quadMesh {
//   rb_x, rb_y, rb_z, rb_u, rb_v, // right bottom
//   rt_x, rt_y, rt_z, rt_u, rt_v, // right top
//   lb_x, lb_y, lb_z, lb_u, lb_v, // left bottom
//
//   lt_x, lt_y, lt_z, lt_u, lt_v, // left top
//   rt_x, rt_y, rt_z, rt_u, rt_v, // right top
//   lb_x, lb_y, lb_z, lb_u, lb_v, // left bottom
// };
typedef std::array<float, 30> quad_mesh;

class Chunk {
  public:
    // Constants
    static constexpr int SIZE { 16 };

    // Instance variables
    glm::vec3 pos; // Center of the chunk
    int blocks[SIZE * SIZE * SIZE] {};

    // Constructor
    Chunk(glm::vec3 pos);
    ~Chunk();

    // Explicitly delete since these shouldn't be used
    Chunk(const Chunk&) = delete; // Delete copy constructor
    Chunk& operator=(const Chunk&) = delete; // Delete copy assignment
    Chunk(Chunk&&) = delete; // Delete move constructor
    Chunk& operator=(Chunk &&) = delete; // Delete move assignment

    // Instance methods
    void render(const Shader &myShader);
    bool isBlockAt(int x, int y, int z) const;

  private:
    unsigned int chunkVAO {};
    unsigned int chunkVBO {};

    std::vector<quad> quads {};
    std::vector<quad_mesh> quadMeshes {};
};
