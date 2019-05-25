#pragma once

#include <cstdint>
#include <bitset>
#include <variant>

namespace platform {
    namespace input
    {
        namespace mouse_data
        {
            struct buttons final {
                std::bitset<16> value;
            };

            struct relative_coords final {
                float x, y;
            };

            struct wheel final {
                float xoffset, yoffset;
            };

            using raw_data = std::variant<
                mouse_data::buttons, mouse_data::relative_coords, mouse_data::wheel
            >;
        }

        namespace keyboard_data
        {
            struct press final {
                std::int32_t key;
                std::int32_t mods;
            };

            struct release final {
                std::int32_t key;
                std::int32_t mods;
            };

            using raw_data = std::variant<keyboard_data::press, keyboard_data::release>;
        }

        using raw_data = std::variant<mouse_data::raw_data, keyboard_data::raw_data>;
    }
    
    struct event_handler {
        virtual ~event_handler() = default;

        virtual void on_resize([[maybe_unused]] std::int32_t width, [[maybe_unused]] std::int32_t height) { };
    };

    struct input_handler {
        virtual ~input_handler() = default;

        virtual void on_update([[maybe_unused]] platform::input::raw_data &raw_data) { };
    };
}
