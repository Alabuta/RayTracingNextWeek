#include "math/math.hxx"


namespace math {
glm::vec3 random_on_unit_sphere(std::mt19937 &generator)
{
    static auto random_distribution = std::uniform_real_distribution{-1.f, +1.f};

    glm::vec3 vector;
    float norm;

    do {
        vector = glm::vec3{
            random_distribution(generator),
            random_distribution(generator),
            random_distribution(generator)
        };

        norm = glm::length2(vector);
    } while (norm < .01f || norm > 1.f);

    return glm::normalize(vector);
}
}
