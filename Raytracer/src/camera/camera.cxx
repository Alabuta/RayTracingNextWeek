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

        auto &&data = camera.data;
        
        /*auto height = std::tan(camera.fov / 2.f);
        auto width = height * camera.aspect;

        data.inverted_projection[0] = glm::vec2{width, 0};
        data.inverted_projection[1] = glm::vec2{0, height};*/

        data.inverted_projection = glm::inverse(glm::perspective(camera.fov, camera.aspect, 1e-3f, 1e3f));
    });
}
}
