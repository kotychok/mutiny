#define BOOST_TEST_MODULE DirectionalLight
#include <boost/test/included/unit_test.hpp>

#include <glm/glm.hpp>

#include "directional_light.h"
#include "print_vector.h"

BOOST_AUTO_TEST_SUITE(DirectionalLightSuite)

BOOST_AUTO_TEST_CASE(symmetricalAngleInDegreesTest) {
  // It returns -180 when time = 0h
  BOOST_TEST(DirectionalLight::symmetricalAngleInDegrees(0) == -180.0f);

  // It returns -90 when time = 6h
  BOOST_TEST(DirectionalLight::symmetricalAngleInDegrees(6) == -90.0f);

  // It returns 0 when time = 12h
  BOOST_TEST(DirectionalLight::symmetricalAngleInDegrees(12) == 0.0f);

  // It returns 90 when time = 18h
  BOOST_TEST(DirectionalLight::symmetricalAngleInDegrees(18) == 90.0f);

  // It returns 180 when time = 24h
  BOOST_TEST(DirectionalLight::symmetricalAngleInDegrees(24) == 180.0f);
}

BOOST_AUTO_TEST_CASE(direction) {
  // It returns pointing in from Bottom when time = 0h (angle = -180 deg)
  BOOST_TEST(DirectionalLight::direction(0).x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(DirectionalLight::direction(0).y == 1.0f);
  BOOST_TEST(DirectionalLight::direction(0).z == 0.0f);
  BOOST_TEST(DirectionalLight::direction(0).w == DIRECTIONAL_LIGHT);

  // It returns pointing in from East when time = 6h (angle = -90 deg)
  BOOST_TEST(DirectionalLight::direction(6).x == -1.0f);
  BOOST_TEST(DirectionalLight::direction(6).y == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(DirectionalLight::direction(6).z == 0.0f);
  BOOST_TEST(DirectionalLight::direction(6).w == DIRECTIONAL_LIGHT);

  // It returns pointing in from Top when time = 12h
  BOOST_TEST(DirectionalLight::direction(12).x == 0.0f);
  BOOST_TEST(DirectionalLight::direction(12).y == -1.0f);
  BOOST_TEST(DirectionalLight::direction(12).z == 0.0f);
  BOOST_TEST(DirectionalLight::direction(12).w == DIRECTIONAL_LIGHT);

  // It returns pointing in from West when time = 18h
  BOOST_TEST(DirectionalLight::direction(18).x == 1.0f);
  BOOST_TEST(DirectionalLight::direction(18).y == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(DirectionalLight::direction(18).z == 0.0f);
  BOOST_TEST(DirectionalLight::direction(18).w == DIRECTIONAL_LIGHT);

  // It returns pointing in from South when time = 24h
  BOOST_TEST(DirectionalLight::direction(24).x == 0.0f, boost::test_tools::tolerance(0.01f));
  BOOST_TEST(DirectionalLight::direction(24).y == 1.0f);
  BOOST_TEST(DirectionalLight::direction(24).z == 0.0f);
  BOOST_TEST(DirectionalLight::direction(24).w == DIRECTIONAL_LIGHT);
}

BOOST_AUTO_TEST_CASE(ambientStrength) {
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
  };

  // It returns 0:
  // * before and at illuminationStart
  // * after and at illuminationEnd
  BOOST_TEST(sun.ambientStrength(illuminationStartAngle - 10) == 0.0f);
  BOOST_TEST(sun.ambientStrength(illuminationStartAngle) == 0.0f);
  BOOST_TEST(sun.ambientStrength(illuminationEndAngle) == 0.0f);
  BOOST_TEST(sun.ambientStrength(illuminationEndAngle + 10) == 0.0f);

  // It returns maxStrength at 0 degrees (overhead)
  BOOST_TEST(sun.ambientStrength(0) == maxAmbientStrength);

  // It slides between those two points
  BOOST_TEST(sun.ambientStrength(illuminationStartAngle / 2) == 0.46875f);
  BOOST_TEST(sun.ambientStrength(illuminationEndAngle / 2) == 0.46875f);
}

BOOST_AUTO_TEST_CASE(diffuseStrength) {
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
  };

  // It returns 0:
  // * before and at illuminationStart
  // * after and at illuminationEnd
  BOOST_TEST(sun.diffuseStrength(illuminationStartAngle - 10) == 0.0f);
  BOOST_TEST(sun.diffuseStrength(illuminationStartAngle) == 0.0f);
  BOOST_TEST(sun.diffuseStrength(illuminationEndAngle) == 0.0f);
  BOOST_TEST(sun.diffuseStrength(illuminationEndAngle + 10) == 0.0f);

  // It returns maxStrength at 0 degrees (overhead)
  BOOST_TEST(sun.diffuseStrength(0) == maxAmbientStrength);

  // It slides between those two points
  BOOST_TEST(sun.diffuseStrength(illuminationStartAngle / 2) == 0.46875f);
  BOOST_TEST(sun.diffuseStrength(illuminationEndAngle / 2) == 0.46875f);
}

BOOST_AUTO_TEST_SUITE_END()
