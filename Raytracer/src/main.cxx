#include "main.hxx"
#include "gfx/context.hxx"
#include "platform/window.hxx"
#include "gfx/shader.hxx"


namespace app {
auto clear_color = std::array{0.f, .254901f, .6f, 1.f};

struct state final {

    std::array<std::int32_t, 2> window_size{800, 600};
};

class window_event_handler final : public platform::events_handler {
public:

    window_event_handler(app::state &app_state) noexcept : app_state{app_state} { }

    void on_resize(std::int32_t width, std::int32_t height) override
    {
        app_state.window_size = std::array{width, height};
    }

private:
    app::state &app_state;
};
}

void render_scene(gfx::framebuffer const &framebuffer)
{
    auto [width, height] = framebuffer.size;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.handle);

    glViewport(0, 0, width, height);

    glClearNamedFramebufferfv(framebuffer.handle, GL_COLOR, 0, std::data(app::clear_color));

    ;
}

int main()
{
#ifdef _DEBUG
    #ifdef _MSC_VER
        _CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #else
        std::signal(SIGSEGV, posix_signal_handler);
        std::signal(SIGTRAP, posix_signal_handler);
    #endif
#endif

    if (auto result = glfwInit(); result != GLFW_TRUE)
        throw std::runtime_error("failed to init GLFW"s);

    app::state app_state{800, 600};

    auto [width, height] = app_state.window_size;

    platform::window window{"Raytracer"sv, width, height};

    auto app_window_event_handler = std::make_shared<app::window_event_handler>(app_state);
    window.connect_handler(app_window_event_handler);

    gfx::context context{window};

    gfx::framebuffer framebuffer{width, height};

    auto shader_stage = gfx::create_shader_stage<gfx::shader::compute>("compute.spv"sv, "main"sv);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    window.update([&app_state, &framebuffer] (auto &&window)
    {
        glfwPollEvents();

        auto [app_width, app_height] = app_state.window_size;
        auto [fbo_width, fbo_height] = framebuffer.size;

        render_scene(framebuffer);

        glBlitNamedFramebuffer(framebuffer.handle, 0, 0, 0, fbo_width, fbo_height, 0, 0, app_width, app_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window.handle());

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));
    });

    glfwTerminate();
}
