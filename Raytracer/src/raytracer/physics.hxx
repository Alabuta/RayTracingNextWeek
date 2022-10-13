#pragma once

#include <vector>

#include "raytracer/primitives.hxx"


namespace physics {
class system final {
public:

    system(std::vector<primitives::sphere> &spheres) noexcept : spheres_{spheres}
    {
        data_.resize(std::size(spheres));
        /*data_.reserve(std::size(spheres));

        std::transform(std::cbegin(spheres), std::cend(spheres), std::back_inserter(data_), [] (auto &&sphere)
        {
            return {sphere.center, glm::vec3{0}};
        });*/
    }

    void update(float delta_time)
    {
        float frames = delta_time / kUPDATE_DELTA_TIME;

        for (auto frame = 0.f; frame < frames; frame += kUPDATE_DELTA_TIME) {
            simulate(kUPDATE_DELTA_TIME);
        }
    }

private:

    static auto constexpr kUPDATE_DELTA_TIME{16e-3f};

    static auto constexpr kGRAVITY{glm::vec3{0, -1.8, 0}};

    std::vector<primitives::sphere> &spheres_;

    struct vel_accel final {
        //glm::vec3 position{0};
        glm::vec3 velocity{0};
    };

    std::vector<vel_accel> data_;

    void simulate(float delta_time)
    {
        auto &&sphere = spheres_.back();
        auto &&data = data_.back();

        auto velocity = data.velocity + kGRAVITY * delta_time;

        sphere.center += (data.velocity + velocity) * .5f * delta_time;
    }
};
}
