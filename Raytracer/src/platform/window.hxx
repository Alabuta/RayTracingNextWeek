#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <functional>

#include <boost/signals2.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "platform.hxx"


namespace platform {
class window final {
public:

    window(std::string_view name, std::int32_t width, std::int32_t height);

    ~window();

    void update(std::function<void(window &)> &&callback);

    GLFWwindow *handle() const noexcept { return handle_; }

    void connect_handler(std::shared_ptr<platform::events_handler> handler);

private:
    GLFWwindow *handle_;

    std::int32_t width_{0}, height_{0};

    std::string name_;

    boost::signals2::signal<void(std::int32_t, std::int32_t)> resize_callback_;

    void set_callbacks();
};
}