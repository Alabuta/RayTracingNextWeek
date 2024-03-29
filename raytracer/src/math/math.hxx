#pragma once

#include <random>
#include <vector>

#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/polar_coordinates.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/norm.hpp>

namespace math {
glm::vec3 random_on_unit_sphere(std::mt19937 &generator);

std::vector<glm::vec3> spherical_fibonacci_lattice(std::size_t number);
}
