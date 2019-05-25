#ifdef _MSC_VER
    #include <execution>
#endif

#include "camera/camera.hxx"


namespace scene {
void camera_system::update()
{
#ifdef _MSC_VER
    std::for_each(std::execution::par_unseq, std::begin(cameras_), std::end(cameras_), [] (auto &&camera_ptr)
#else
    std::for_each(std::begin(cameras_), std::end(cameras_), [] (auto &&camera_ptr)
#endif
    {
        auto &&camera = *camera_ptr;
        
        auto theta = camera.fov / 2.f;

        auto height = std::tan(theta);
        auto width = height * camera.aspect;

        auto &&data = camera.data;

        data.projection[0] = glm::vec2{width, 0};
        data.projection[1] = glm::vec2{0, height};
    });
}
}
