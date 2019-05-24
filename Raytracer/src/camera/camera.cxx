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
        
        auto theta = glm::radians(camera.fov) / 2.f;

        auto height = std::tan(theta);
        auto width = height * camera.aspect;

        auto &&data = camera.data;

        data.lower_left_corner = data.origin - (camera.u * width + camera.v * height + camera.w);

        data.horizontal = camera.u * width * 2.f;
        data.vertical = camera.v * height * 2.f;
    });
}
}
