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

    /* auto &&world = camera_->world;

    world = glm::inverse(glm::lookAt(offset_, target_, {0, 1, 0})); */
}

void OrbitController::rotate(float longitude, float latitude)
{
    auto speed = (1.f - damping_) * .008f;

    polarDelta_.x += latitude * speed;
    polarDelta_.y -= longitude * speed;
}

void OrbitController::pan(float x, float y)
{
    auto speed = (1.f - damping_) * .001f;

    panDelta_.x -= x * speed;
    panDelta_.y += y * speed;
}

void OrbitController::dolly(float delta)
{
    auto speed = (1.f - damping_) * 2.f;

    auto dollying = std::pow(.95f, std::abs(delta) * speed);

    scale_ = std::signbit(delta) ? (std::signbit(delta) ? 1.f / dollying : 1.f) : dollying;
}

void OrbitController::update()
{
#if 0
    auto &&world = camera_->world;

    auto xAxis = glm::vec3{world[0]};
    auto yAxis = glm::vec3{world[1]};
    // auto zAxis = glm::vec3{world[2]};

    auto position = glm::vec3{world[3]};

    offset_ = position - target_;

    auto radius = glm::length(offset_);
    auto distance = radius * 2.f * std::tan(camera_->fov * .5f);

    radius = std::clamp(radius * scale_, minZ, maxZ);

    xAxis *= panDelta_.x * distance;
    yAxis *= panDelta_.y * distance;

    panOffset_ = xAxis + yAxis;

    polar_ = glm::polar(offset_);
    polar_ = glm::clamp(polar_ + polarDelta_, min_polar, max_polar);

    offset_ = glm::euclidean(polar_) * radius;

    /*auto yaw = std::atan2(zAxis.x, zAxis.z);
    orientation_ = glm::angleAxis(yaw, up_);
    zAxis = orientation_ * (direction_lerped_ * glm::vec3{1, 0, -1});
    panOffset_ += zAxis * std::max(.1f, std::log(std::abs(distance) + 1.f));*/

    target_ += panOffset_;

    position = target_ + offset_;

    world = glm::inverse(glm::lookAt(position, target_, up_));
#endif

#if 0
    auto orientation = from_two_vec3(camera_->up, glm::vec3{0, 1, 0});
#endif

    apply_damping();
}

void OrbitController::apply_damping()
{
    polarDelta_ *= damping_;

    panDelta_ *= damping_;

    scale_ += (1 - scale_) * (1 - damping_);

    direction_lerped_ = glm::mix(direction_lerped_, direction_, 1.f - damping_);
}
