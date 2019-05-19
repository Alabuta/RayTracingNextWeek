#pragma once

#include <cstdint>

namespace platform {
struct events_handler {
    virtual ~events_handler() = default;

    virtual void on_resize(std::int32_t width, std::int32_t height) { };
};
}
