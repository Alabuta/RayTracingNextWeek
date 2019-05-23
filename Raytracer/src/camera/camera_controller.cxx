#ifdef _MSC_VER
    #define USE_EXECUTION_POLICIES
    #include <execution>
#endif

#include "camera/input_handlers/mouse_handler.hxx"
#include "camera/camera.hxx"


template<class U, class V>
glm::quat from_two_vec3(U &&u, V &&v)
{
    auto norm_uv = std::sqrt(glm::dot(u, u) * glm::dot(v, v));
    auto real_part = norm_uv + glm::dot(u, v);

    glm::vec3 w{0};

    if (real_part < 1.e-6f * norm_uv) {
        real_part = 0.f;

        w = std::abs(u.x) > std::abs(u.z) ? glm::vec3{-u.y, u.x, 0} : glm::vec3{0, -u.z, u.y};
    }

    else w = glm::cross(u, v);

    return glm::normalize(glm::quat{real_part, w});
}

OrbitController::OrbitController(std::shared_ptr<scene::camera> camera, input::input_manager &input_manager) : camera_{camera}
{
    mouse_handler_ = std::make_shared<mouse_handler>(*this);
    input_manager.mouse().connect(mouse_handler_);
}

void OrbitController::look_at(glm::vec3 &&eye, glm::vec3 &&target)
{
    target_ = target;
    offset_ = eye;

    camera_->w = glm::normalize(eye - target);
    camera_->u = glm::normalize(glm::cross(camera_->up, camera_->w));
    camera_->v = glm::normalize(glm::cross(camera_->w, camera_->u));

    camera_->data.origin = std::move(eye);

    auto &&world = camera_->world;

    world = glm::inverse(glm::lookAt(offset_, target_, camera_->up));
}

void OrbitController::rotate(float longitude, float latitude)
{
    auto speed = (1.f - damping_) * .008f;

    polar_delta_.x += latitude * speed;
    polar_delta_.y -= longitude * speed;
}

void OrbitController::pan(float x, float y)
{
    auto speed = (1.f - damping_) * .001f;

    pan_delta_.x -= x * speed;
    pan_delta_.y += y * speed;
}

void OrbitController::dolly(float delta)
{
    auto speed = (1.f - damping_) * 2.f;

    auto dollying = std::pow(.95f, std::abs(delta) * speed);

    scale_ = std::signbit(delta) ? (std::signbit(delta) ? 1.f / dollying : 1.f) : dollying;
}

void OrbitController::update()
{
    auto &&world = camera_->world;

    auto xAxis = glm::vec3{world[0]};
    auto yAxis = glm::vec3{world[1]};
    // auto zAxis = glm::vec3{world[2]};

    auto position = glm::vec3{world[3]};

    offset_ = position - target_;

    auto radius = glm::length(offset_);
    auto distance = radius * 2.f * std::tan(camera_->fov * .5f);

    radius = std::clamp(radius * scale_, minZ, maxZ);

    xAxis *= pan_delta_.x * distance;
    yAxis *= pan_delta_.y * distance;

    pan_offset_ = xAxis + yAxis;

    polar_ = glm::polar(offset_);
    polar_ = glm::clamp(polar_ + polar_delta_, min_polar, max_polar);

    offset_ = glm::euclidean(polar_) * radius;

    /*auto yaw = std::atan2(zAxis.x, zAxis.z);
    orientation_ = glm::angleAxis(yaw, up_);
    zAxis = orientation_ * (direction_lerped_ * glm::vec3{1, 0, -1});
    panOffset_ += zAxis * std::max(.1f, std::log(std::abs(distance) + 1.f));*/

    target_ += pan_offset_;

    position = target_ + offset_;

    world = glm::inverse(glm::lookAt(position, target_, up_));

#if 0
    auto orientation = from_two_vec3(camera_->up, glm::vec3{0, 1, 0});
#endif

    apply_damping();
}

void OrbitController::apply_damping()
{
    polar_delta_ *= damping_;

    pan_delta_ *= damping_;

    scale_ += (1 - scale_) * (1 - damping_);

    direction_lerped_ = glm::mix(direction_lerped_, direction_, 1.f - damping_);
}
