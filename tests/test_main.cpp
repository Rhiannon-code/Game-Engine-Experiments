// doctest provides its own main() here. As the engine grows, split tests into
// files per subsystem (math, ecs, serialization) and link them into this target.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("glm: translation moves the origin") {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 4.0f, 0.0f));
    glm::vec4 p = m * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    CHECK(p.x == doctest::Approx(3.0f));
    CHECK(p.y == doctest::Approx(4.0f));
}
