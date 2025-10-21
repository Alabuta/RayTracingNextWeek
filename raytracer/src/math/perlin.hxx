#pragma once


namespace math
{
    struct perlin final {
        std::array<std::uint32_t, 256> x;
        std::array<std::uint32_t, 256> y;
        std::array<std::uint32_t, 256> z;
        std::array<glm::vec3, 256> randoms;
    };

    math::perlin create_perlin_noise();
}
