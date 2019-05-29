#include "input/keyboard.hxx"

namespace {
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
}


namespace input {
void keyboard::connect(std::shared_ptr<keyboard::handler> slot)
{
    on_press_.connect(decltype(on_press_)::slot_type{
        &handler::on_press, slot.get(), _1
    }.track_foreign(slot));

    on_release_.connect(decltype(on_release_)::slot_type{
        &handler::on_release, slot.get(), _1
    }.track_foreign(slot));
}

void keyboard::update(platform::input::keyboard_data::raw_data &data)
{
    std::visit(overloaded{
        [this] (platform::input::keyboard_data::press &press)
        {
            [[maybe_unused]] auto [key, mods] = press;


            //if (std::find(std::cbegin(pressed_.set), std::cend(pressed_.set), static_cast<handler::key>(key)))

            //pressed_[0] = static_cast<std::int16_t>(key);

            on_press_(pressed_);
        },
        [this] (platform::input::keyboard_data::release &release)
        {
            [[maybe_unused]] auto [key, mods] = release;

            //pressed_[0] = -1;

            on_release_(pressed_);
        },
        [] (auto &&) { }
    }, data);
}
}
