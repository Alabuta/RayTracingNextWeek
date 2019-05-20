#pragma once

#include <cstdint>

namespace platform {
struct events_handler {
    virtual ~events_handler() = default;

    virtual void on_resize([[maybe_unused]] std::int32_t width, [[maybe_unused]] std::int32_t height) { };
};
}
