#include <cmath>

#include "math/math.hxx"


namespace math {
glm::vec3 random_on_unit_sphere(std::mt19937 &generator)
{
    glm::vec3 vector;

#if 0
    static auto random_distribution = std::uniform_real_distribution{-1.f, +1.f};

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
#else
    static auto random_distribution = std::uniform_real_distribution<float>{0.f, 1.f};

    auto phi = random_distribution(generator) * glm::pi<float>() * 2.f;
    auto cos_theta = random_distribution(generator) * 2.f - 1.f;

    auto theta = std::acos(cos_theta);

    auto sin_theta = std::sin(theta);

    vector.x = sin_theta * std::cos(phi);
    vector.y = sin_theta * std::sin(phi);
    vector.z = cos_theta;

    return vector;
#endif
}

std::vector<glm::vec3> spherical_fibonacci_lattice(std::size_t number)
{
    std::vector<glm::vec3> points(number);

    auto const random = 1.f;
    auto const offset = 2.f / static_cast<float>(number);
    auto const increment = glm::pi<float>() * (3.f - std::sqrt(5.f));

    std::generate(std::begin(points), std::end(points), [&, i = 0.f] () mutable
    {
        auto y = (i + .5f) * offset - 1.f;

        auto r = std::sqrt(1.f - y * y);

        auto phi = std::fmod(i + random, static_cast<float>(number)) * increment;

        auto x = std::cos(phi) * r;
        auto z = std::sin(phi) * r;

        ++i;

        return glm::vec3{x, y, z};
    });

    return points;
}
}
