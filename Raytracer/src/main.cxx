#include "main.hxx"
#include "gfx/context.hxx"
#include "platform/window.hxx"
#include "gfx/framebuffer.hxx"
#include "gfx/render_pass.hxx"
#include "gfx/shader.hxx"
#include "gfx/image.hxx"


namespace app {
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



void render_scene(gfx::render_pass const &render_pass)
{
    auto [vao, framebuffer] = render_pass;

    auto [width, height] = framebuffer.size;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.handle);

    glViewport(0, 0, width, height);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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

    app::state app_state{512, 512};

    auto [width, height] = app_state.window_size;

    platform::window window{"Raytracer"sv, width, height};

    auto app_window_event_handler = std::make_shared<app::window_event_handler>(app_state);
    window.connect_handler(app_window_event_handler);

    gfx::context context{window};

    auto image = gfx::create_image2D(width, height, GL_RGBA32F);

    glBindImageTexture(2, image.handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTextureUnit(4, image.handle);

    gfx::render_pass render_pass;

    {
        auto attachment = gfx::create_image2D(width, height, GL_SRGB8_ALPHA8);
        auto framebuffer = gfx::create_framebuffer(width, height, attachment);

        render_pass = gfx::create_render_pass(std::move(framebuffer));
    }

    gfx::program compute_program;

    {
        auto compute_shader_stage = gfx::create_shader_stage<gfx::shader::compute>("shader.comp.spv"sv, "main"sv);
        compute_program = gfx::create_program(std::vector{compute_shader_stage});
    }

    gfx::program screen_quad_program;

    {
        auto vertex_stage = gfx::create_shader_stage<gfx::shader::vertex>("shader.vert.spv"sv, "main"sv);
        auto fragment_stage = gfx::create_shader_stage<gfx::shader::fragment>("shader.frag.spv"sv, "main"sv);

        screen_quad_program = gfx::create_program(std::vector{vertex_stage, fragment_stage});
    }

    {
        std::int32_t max_compute_work_group_invocations = -1;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

        std::cout << "\nGL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: "s << max_compute_work_group_invocations;
        std::cout << "\nGL_MAX_COMPUTE_WORK_GROUP_COUNT: "s;

        for (auto index : {0u, 1u, 2u}) {
            std::int32_t max_compute_work_group_count = -1;
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index, &max_compute_work_group_count);

            std::cout << max_compute_work_group_count << ' ';
        }

        std::cout << "\nGL_MAX_COMPUTE_WORK_GROUP_SIZE: "s;

        for (auto index : {0u, 1u, 2u}) {
            std::int32_t max_compute_work_group_size = -1;
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, index, &max_compute_work_group_size);

            std::cout << max_compute_work_group_size << ' ';
        }

        std::cout << std::endl;
    }

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    window.update([&app_state, &render_pass, &compute_program, &screen_quad_program] (auto &&window)
    {
        glfwPollEvents();

        auto [fbo_width, fbo_height] = render_pass.framebuffer.size;

        auto start = std::chrono::system_clock::now();

        glUseProgram(compute_program.handle);
        glDispatchCompute(static_cast<std::uint32_t>(fbo_width) / 16, static_cast<std::uint32_t>(fbo_height) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(screen_quad_program.handle);
        render_scene(render_pass);

        auto [app_width, app_height] = app_state.window_size;

        glBlitNamedFramebuffer(render_pass.framebuffer.handle, 0, 0, 0, fbo_width, fbo_height, 0, 0, app_width, app_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window.handle());

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

        glfwSetWindowTitle(window.handle(), std::to_string(duration.count()).c_str());

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));
    });

    glfwTerminate();
}
