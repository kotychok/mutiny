#define BOOST_TEST_MODULE DirectionalLight
#include <boost/test/included/unit_test.hpp>

#include <glm/glm.hpp>

#include "directional_light.h"
#include "print_vector.h"

float maxAmbientStrength = 0.5f;
float illuminationStartAngle = -108.0f;
float illuminationEndAngle = 108.0f;
DirectionalLight sun {
  glm::vec3(1.0f, 1.0f, 1.0f), // color
  maxAmbientStrength,
  illuminationStartAngle,
  illuminationEndAngle,
  1.0f,  // ambientOverride
  1.0f,  // diffuseOverride
  1.0f,  // specularOverride
  false, // useOverrides
  0     // offset
};

DirectionalLight moon {
  glm::vec3(1.0f, 1.0f, 1.0f), // color
  maxAmbientStrength,
  illuminationStartAngle,
  illuminationEndAngle,
  1.0f,  // ambientOverride
  1.0f,  // diffuseOverride
  1.0f,  // specularOverride
  false, // useOverrides
  180    // offset
};

BOOST_AUTO_TEST_SUITE(DirectionalLightSuite)

BOOST_AUTO_TEST_CASE(angleToTime) {
  BOOST_TEST(sun.angleToTime(-180) == 0.0f);
  BOOST_TEST(sun.angleToTime(-90) == 6.0f);
  BOOST_TEST(sun.angleToTime(0) == 12.0f);
  BOOST_TEST(sun.angleToTime(90) == 18.0f);
  BOOST_TEST(sun.angleToTime(180) == 0.0f);

  BOOST_TEST(moon.angleToTime(-180) == 12.0f);
  BOOST_TEST(moon.angleToTime(-90) == 18.0f);
  BOOST_TEST(moon.angleToTime(0) == 0.0f);
  BOOST_TEST(moon.angleToTime(90) == 6.0f);
  BOOST_TEST(moon.angleToTime(180) == 12.0f);
}

BOOST_AUTO_TEST_CASE(angle) {
  BOOST_TEST(sun.angle(0) == 180.0f);
  BOOST_TEST(sun.angle(6) == 90.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(sun.angle(12) == 0.0f);
  BOOST_TEST(sun.angle(18) == 90.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(sun.angle(24) == 180.0f);

  BOOST_TEST(moon.angle(0) == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(moon.angle(6) == 90.0f);
  BOOST_TEST(moon.angle(12) == 180.0f);
  BOOST_TEST(moon.angle(18) == 90.0f);
  BOOST_TEST(moon.angle(24) == 0.0f);
}

BOOST_AUTO_TEST_CASE(direction) {
  glm::vec4 direction {};

  // ******* SUN *******

  // It returns pointing in from Bottom when time = 0h (angle = -180 deg)
  direction = sun.direction(0);
  BOOST_TEST(direction.x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.y == 1.0f);
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from East when time = 6h (angle = -90 deg)
  direction = sun.direction(6);
  BOOST_TEST(direction.x == -1.0f);
  BOOST_TEST(direction.y == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from Top when time = 12h
  direction = sun.direction(12);
  BOOST_TEST(direction.x == 0.0f);
  BOOST_TEST(direction.y == -1.0f);
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from West when time = 18h
  direction = sun.direction(18);
  BOOST_TEST(direction.x == 1.0f);
  BOOST_TEST(direction.y == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from South when time = 24h
  direction = sun.direction(24);
  BOOST_TEST(direction.x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.y == 1.0f);
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // ******* MOON *******

  // It returns pointing in from top when time = 0h (angle = 0 deg)
  direction = moon.direction(0);
  BOOST_TEST(direction.x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.y == -1.0f);
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from west when time = 6h (angle = 90 deg)
  direction = moon.direction(6);
  BOOST_TEST(direction.x == 1.0f);
  BOOST_TEST(direction.y == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from bottom when time = 12h (angle = 180 deg)
  direction = moon.direction(12);
  BOOST_TEST(direction.x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.y == 1.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from east when time = 18h (angle = -90 deg)
  direction = moon.direction(18);
  BOOST_TEST(direction.x == -1.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.y == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);

  // It returns pointing in from top when time = 24h (angle = -180 deg)
  direction = moon.direction(24);
  BOOST_TEST(direction.x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.y == -1.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(direction.z == 0.0f);
  BOOST_TEST(direction.w == DIRECTIONAL_LIGHT);
}

BOOST_AUTO_TEST_CASE(ambientStrength) {
  // It returns 0:
  // * before and at illuminationStart
  // * after and at illuminationEnd
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(illuminationStartAngle - 10)) == 0.0f);
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(illuminationStartAngle)) == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(illuminationEndAngle)) == 0.0f);
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(illuminationEndAngle + 10)) == 0.0f);

  // It returns maxStrength at 0 degrees (overhead)
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(0)) == maxAmbientStrength);

  // It slides between those two points
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(illuminationStartAngle / 2)) == 0.46875f, boost::test_tools::tolerance(0.00001f));
  BOOST_TEST(sun.ambientStrength(sun.angleToTime(illuminationEndAngle / 2)) == 0.46875f, boost::test_tools::tolerance(0.00001f));
}

BOOST_AUTO_TEST_SUITE_END()
