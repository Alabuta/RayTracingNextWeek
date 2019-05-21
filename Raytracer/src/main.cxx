#include "main.hxx"

#include "gfx/context.hxx"
#include "platform/window.hxx"

#include "gfx/framebuffer.hxx"
#include "gfx/render_pass.hxx"
#include "gfx/shader.hxx"
#include "gfx/image.hxx"

#include "camera/camera.hxx"


auto constexpr kCAMERA_BINDING = 7;
auto constexpr kOUT_IMAGE_BINDING = 2;


namespace app {
struct state final {

    std::array<std::int32_t, 2> window_size{0, 0};
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

    auto aspect = static_cast<float>(width) / height;

    auto camera = scene::camera{glm::vec3{0, 0, 0}, glm::vec3{0, 0, -1}, glm::vec3{0, 1, 0}, 90.f, aspect};

    platform::window window{"Raytracer"sv, width, height};

    auto app_window_event_handler = std::make_shared<app::window_event_handler>(app_state);
    window.connect_handler(app_window_event_handler);

    gfx::context context{window};

    auto image = gfx::create_image2D(width, height, GL_RGBA32F);

    glBindImageTexture(kOUT_IMAGE_BINDING, image.handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
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
        glUseProgram(compute_program.handle);
        auto index = glGetProgramResourceIndex(compute_program.handle, GL_SHADER_STORAGE_BLOCK, "PER_VIEW");

        if (index == GL_INVALID_INDEX)
            throw std::runtime_error("can't init the SSBO - invalid index param"s);

        std::uint32_t camera_ssbo_handle = 0;

        glCreateBuffers(1, &camera_ssbo_handle);
        glObjectLabel(GL_BUFFER, camera_ssbo_handle, -1, "[BO]");

        glNamedBufferStorage(camera_ssbo_handle, sizeof(scene::camera::gpu_data), &camera.data, GL_DYNAMIC_STORAGE_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kCAMERA_BINDING, camera_ssbo_handle);
        //glShaderStorageBlockBinding(compute_program.handle, index, kCAMERA_BINDING);

        glMemoryBarrier(GL_ALL_BARRIER_BITS);
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
