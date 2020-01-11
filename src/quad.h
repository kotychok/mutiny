#pragma once

#include <array>

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
