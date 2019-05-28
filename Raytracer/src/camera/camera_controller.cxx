#ifdef _MSC_VER
    #define USE_EXECUTION_POLICIES
    #include <execution>
#endif

#include "camera/input_handlers/mouse_handler.hxx"
#include "camera/camera.hxx"


namespace camera {
orbit_controller::orbit_controller(std::shared_ptr<scene::camera> camera, input::input_manager &input_manager) : camera_{camera}
{
    mouse_handler_ = std::make_shared<mouse_handler>(*this);
    input_manager.mouse().connect(mouse_handler_);
}

void orbit_controller::look_at(glm::vec3 const &eye, glm::vec3 const &target)
{
    target_ = target;
    offset_ = eye;

    camera_->data.world = glm::inverse(glm::lookAt(offset_, target_, {0, 1, 0}));
}

void orbit_controller::update()
{
    auto &&world = camera_->data.world;

    auto xAxis = glm::normalize(glm::vec3{world[0]});
    auto yAxis = glm::normalize(glm::vec3{world[1]});
    //auto zAxis = glm::normalize(glm::vec3{world[2]});

    auto position = glm::vec3{world[3]};

    offset_ = position - target_;

    if (glm::distance(offset_, glm::zero<glm::vec3>()) <= std::numeric_limits<float>::min()) {
        position = target_ + glm::vec3{0, 0, std::numeric_limits<float>::min()};

        offset_ = position - target_;
    }

    auto radius = glm::length(offset_);
    auto distance = radius * std::tan(camera_->fov * .5f) * 2.f;

    radius = std::clamp(radius * scale_, min_z, max_z);

    xAxis *= pan_delta_.x * distance;
    yAxis *= pan_delta_.y * distance;

    pan_offset_ = xAxis + yAxis;

    polar_ = glm::polar(offset_);
    polar_ = glm::clamp(polar_ + polar_delta_, min_polar, max_polar);

    offset_ = glm::euclidean(polar_) * radius;

    target_ += pan_offset_;

    position = target_ + offset_;

    world = glm::inverse(glm::lookAt(position, target_, up_));

    apply_damping();
}

void orbit_controller::rotate(float longitude, float latitude)
{
    auto speed = (1.f - damping_) * .0064f;

    polar_delta_.x -= latitude * speed;
    polar_delta_.y += longitude * speed;
}

void orbit_controller::pan(float x, float y)
{
    auto speed = (1.f - damping_) * .0024f;

    pan_delta_.x += x * speed;
    pan_delta_.y -= y * speed;
}

void orbit_controller::dolly(float delta)
{
    auto speed = (1.f - damping_) * 1.6f;

    auto dollying = std::pow(.95f, std::abs(delta) * speed);

    scale_ = std::signbit(delta) ? (std::signbit(delta) ? 1.f / dollying : 1.f) : dollying;
}

void orbit_controller::apply_damping()
{
    polar_delta_ *= damping_;

    pan_delta_ *= damping_;

    scale_ += (1 - scale_) * (1 - damping_);

    direction_lerped_ = glm::mix(direction_lerped_, direction_, 1.f - damping_);
}
}
