#define BOOST_TEST_MODULE MesherGreedy
#include <boost/test/included/unit_test.hpp>

#include "chunk.h"
#include "mesher_greedy.h"

BOOST_AUTO_TEST_CASE(chunkToQuads) {
  glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
  Chunk chunk = Chunk(pos);
  std::vector<quad> actual { MesherGreedy::chunkToQuads(chunk) };
  std::vector<quad> expected {
    quad{-16, -1, -16, -16, 0, -16, -16, -1, 16, -16, 0, 16},
    quad{16, -1, -16, 16, 0, -16, 16, -1, 16, 16, 0, 16},
    quad{-16, -1, -16, -16, -1, 16, 16, -1, -16, 16, -1, 16},
    quad{-16, 0, -16, -16, 0, 16, 16, 0, -16, 16, 0, 16},
    quad{-16, -1, -16, 16, -1, -16, -16, 0, -16, 16, 0, -16},
    quad{-16, -1, 16, 16, -1, 16, -16, 0, 16, 16, 0, 16},
  };

  BOOST_TEST(actual == expected);
}
