#include "main.hxx"
#include "gfx/context.hxx"
#include "platform/window.hxx"


namespace app {
auto clear_color = std::array{0.f, .254901f, .6f, 1.f};

struct properties final {

    std::array<std::int32_t, 2> window_size{800, 600};
};

class window_event_handler final : public platform::events_handler {
public:

    window_event_handler(app::properties &app_properties) noexcept : app_properties{app_properties} { }

    void on_resize(std::int32_t width, std::int32_t height) override
    {
        app_properties.window_size = std::array{width, height};
    }

private:
    app::properties &app_properties;
};
}


int main()
{
#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
    std::signal(SIGSEGV, posix_signal_handler);
    std::signal(SIGTRAP, posix_signal_handler);
#endif

    if (auto result = glfwInit(); result != GLFW_TRUE)
        throw std::runtime_error("failed to init GLFW"s);

    app::properties app_properties{800, 600};

    auto [width, height] = app_properties.window_size;

    platform::window window{"Raytracer"sv, width, height};

    auto app_window_event_handler = std::make_shared<app::window_event_handler>(app_properties);
    window.connect_handler(app_window_event_handler);

    gfx::context context{window};

    gfx::framebuffer framebuffer{width, height};

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    window.update([&] (auto &&window)
    {
        glfwPollEvents();

        auto [width, height] = app_properties.window_size;

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.handle);

        glClearNamedFramebufferfv(framebuffer.handle, GL_COLOR, 0, std::data(app::clear_color));
        glClearNamedFramebufferfi(framebuffer.handle, GL_DEPTH_STENCIL, 0, 1.f, 1);

        glViewport(0, 0, width, height);

        glBlitNamedFramebuffer(framebuffer.handle, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window.handle());

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));
    });

    glfwTerminate();
}
