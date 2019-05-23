#include "math/math.hxx"


namespace math {
glm::vec3 random_in_unit_sphere(std::mt19937 &generator)
{
    static auto random_distribution = std::uniform_real_distribution{-1.f, +1.f};

    glm::vec3 vector;

    do {
        vector = glm::vec3{random_distribution(generator), random_distribution(generator), random_distribution(generator)};
    } while (glm::length(vector) > 1.f);

    return vector;
}
}
