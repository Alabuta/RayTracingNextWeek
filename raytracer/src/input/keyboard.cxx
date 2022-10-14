#include <algorithm>
#include "input/keyboard.hxx"

namespace {
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
}


namespace input {
keyboard::keyboard()
{
    keys_state_ = std::make_unique<keyboard::handler::keys_state>(keys_);
}

void keyboard::connect(std::shared_ptr<keyboard::handler> slot)
{
    using boost::placeholders::_1;

    state_on_press_.connect(decltype(state_on_press_)::slot_type{
        &handler::state_on_press, slot.get(), _1
    }.track_foreign(slot));

    state_on_release_.connect(decltype(state_on_release_)::slot_type{
        &handler::state_on_release, slot.get(), _1
    }.track_foreign(slot));

    on_press_key_.connect(decltype(on_press_key_)::slot_type{
        &handler::on_press_key, slot.get(), _1
    }.track_foreign(slot));

    on_release_key_.connect(decltype(on_release_key_)::slot_type{
        &handler::on_release_key, slot.get(), _1
    }.track_foreign(slot));
}

void keyboard::update(platform::input::keyboard_data::raw_data &data)
{
    std::visit(overloaded{
        [this] (platform::input::keyboard_data::press &press)
        {
            [[maybe_unused]] auto [key, mods] = press;

            auto _key = static_cast<handler::key>(key);

            if (!(*keys_state_)(_key)) {
                on_press_key_(_key);

                keys_.emplace(_key);
            }

            state_on_press_(*keys_state_);
        },
        [this] (platform::input::keyboard_data::release &release)
        {
            [[maybe_unused]] auto [key, mods] = release;

            auto _key = static_cast<handler::key>(key);

            if (!(*keys_state_)(_key)) {
                on_release_key_(_key);

                keys_.erase(_key);
            }

            state_on_release_(*keys_state_);
        },
        [] (auto &&) { }
    }, data);
}
}
