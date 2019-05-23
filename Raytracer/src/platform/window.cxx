#include <stdexcept>

#include <string>
using namespace std::string_literals;

#include "window.hxx"


namespace platform {
window::window(std::string_view name, std::int32_t width, std::int32_t height)
    : handle_{nullptr}, width_{width}, height_{height}, name_{name}
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //#if defined(_DEBUG)
        // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    //#else
        // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
    //#endif

    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    handle_ = glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);

    if (handle_ == nullptr)
        throw std::runtime_error("failed to create '"s + name_ + "' window"s);

    glfwSetWindowUserPointer(handle_, this);

    set_callbacks();
}

window::~window()
{
    if (handle_)
        glfwDestroyWindow(handle_);
}

void window::connect_handler(std::shared_ptr<platform::event_handler> handler)
{
    resize_callback_.connect(decltype(resize_callback_)::slot_type(
        &event_handler::on_resize, handler.get(), _1, _2
    ).track_foreign(handler));
}

void window::connect_handler(std::shared_ptr<platform::input_handler> handler)
{
    input_update_callback_.connect(decltype(input_update_callback_)::slot_type(
        &input_handler::on_update, handler.get(), _1
    ).track_foreign(handler));
}

void window::update(std::function<void(window &)> &&callback)
{
    while (glfwWindowShouldClose(handle_) == GLFW_FALSE && glfwGetKey(handle_, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        callback(*this);
    }
}

void window::set_callbacks()
{
    glfwSetWindowSizeCallback(handle_, [] (auto handle, auto width, auto height)
    {
        auto instance = reinterpret_cast<window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            instance->width_ = width;
            instance->height_ = height;

            instance->resize_callback_(width, height);
        }
    });

    glfwSetCursorPosCallback(handle_, [] (auto handle, auto x, auto y)
    {
        auto instance = reinterpret_cast<window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            auto coords = input::mouse_input_data::relative_coords{
                static_cast<decltype(input::mouse_input_data::relative_coords::x)>(x),
                static_cast<decltype(input::mouse_input_data::relative_coords::y)>(y)
            };

            input::raw_data raw_data = input::mouse_input_data::raw_data{std::move(coords)};

            instance->input_update_callback_(raw_data);
        }
    });

    glfwSetMouseButtonCallback(handle_, [] (auto handle, auto button, auto action, auto)
    {
        auto instance = reinterpret_cast<window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            auto buttons = input::mouse_input_data::buttons{};

            std::size_t offset = action == GLFW_PRESS ? 0 : 1;

            buttons.value[static_cast<std::size_t>(button) * 2 + offset] = 1;

            input::raw_data data = input::mouse_input_data::raw_data{std::move(buttons)};

            instance->input_update_callback_(data);
        }
    });

    // TODO: replace to 'glfwSetMouseWheelCallback'
    glfwSetScrollCallback(handle_, [] (auto handle, auto xoffset, auto yoffset)
    {
        auto instance = reinterpret_cast<window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            auto wheel = input::mouse_input_data::wheel{
                static_cast<decltype(input::mouse_input_data::wheel::xoffset)>(xoffset),
                static_cast<decltype(input::mouse_input_data::wheel::yoffset)>(yoffset)
            };

            input::raw_data data = input::mouse_input_data::raw_data{std::move(wheel)};

            instance->input_update_callback_(data);
        }
    });
}
}
