#include <array>
#include <algorithm>

#include "math/math.hxx"
#include "math/perlin.hxx"

namespace math
{
    math::perlin create_perlin_noise()
    {
        math::perlin perlin{};

        std::iota(std::begin(perlin.x), std::end(perlin.x), 0u);
        std::iota(std::begin(perlin.y), std::end(perlin.y), 0u);
        std::iota(std::begin(perlin.z), std::end(perlin.z), 0u);

        std::random_device random_device;
        std::mt19937 generator{ random_device() };

        auto real_distribution = std::uniform_real_distribution<float>{ 0.f, 1.f };

        std::transform(std::rbegin(perlin.x), std::rend(perlin.x), std::rbegin(perlin.x), [&generator, &real_distribution, &perlin, i = 256u](auto &&x) mutable
        {
            auto target = static_cast<std::uint32_t>(real_distribution(generator) * static_cast<float>(--i));

            std::swap(x, perlin.x[target]);

            return x;
        });

        std::transform(std::rbegin(perlin.y), std::rend(perlin.y), std::rbegin(perlin.y), [&generator, &real_distribution, &perlin, i = 256u](auto &&y) mutable
        {
            auto target = static_cast<std::uint32_t>(real_distribution(generator) * static_cast<float>(--i));

            std::swap(y, perlin.y[target]);

            return y;
        });

        std::transform(std::rbegin(perlin.z), std::rend(perlin.z), std::rbegin(perlin.z), [&generator, &real_distribution, &perlin, i = 256u](auto &&z) mutable
        {
            auto target = static_cast<std::uint32_t>(real_distribution(generator) * static_cast<float>(--i));

            std::swap(z, perlin.z[target]);

            return z;
        });

        //std::generate(std::begin(perlin.randoms), std::end(perlin.randoms), [&generator, &real_distribution]
        //{
        //    //return real_distribution(generator);
        //    return glm::normalize(math::random_on_unit_sphere(generator));
        //});
        auto unit_vectors = math::spherical_fibonacci_lattice(256);
        std::move(std::begin(unit_vectors), std::end(unit_vectors), std::begin(perlin.randoms));

        return perlin;
    }
}
