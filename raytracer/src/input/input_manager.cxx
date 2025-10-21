#include "platform/platform.hxx"
#include "input/input_manager.hxx"


namespace {
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

namespace input {
void input_manager::on_update(platform::input::raw_data &raw_data)
{
    std::visit(overloaded{
        [this] (platform::input::mouse_data::raw_data &data)
        {
            mouse_.update(data);
        },
        [this] (platform::input::keyboard_data::raw_data &data)
        {
            keyboard_.update(data);
        },
        [] (auto &&) { }
    }, raw_data);
}
}
