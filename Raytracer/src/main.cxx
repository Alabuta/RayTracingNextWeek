#include "main.hxx"

#include "gfx/context.hxx"
#include "platform/window.hxx"

#include "gfx/framebuffer.hxx"
#include "gfx/render_pass.hxx"
#include "gfx/shader.hxx"
#include "gfx/image.hxx"

#include "camera/camera.hxx"

#include "raytracer/primitives.hxx"


auto constexpr kOUT_IMAGE_BINDING = 2u;
auto constexpr kUNIT_VECTORS_BUFFER_BINDING = 5u;
auto constexpr kPRIMITIVES_BINDING = 6u;
auto constexpr kCAMERA_BINDING = 7u;

auto constexpr kUNIT_VECTORS_BUFFER_SIZE = 1'000;


namespace app {
struct state final {
    std::array<std::int32_t, 2> window_size{0, 0};

    scene::camera camera;
};

class window_event_handler final : public platform::events_handler {
public:

    window_event_handler(app::state &app_state) noexcept : app_state{app_state} { }

    void on_resize(std::int32_t width, std::int32_t height) override
    {
        app_state.window_size = std::array{width, height};

        app_state.camera.aspect = static_cast<float>(width) / static_cast<float>(height);
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

    app::state app_state{512, 512, scene::camera{}};

    auto [width, height] = app_state.window_size;

    auto const grid_size_x = static_cast<std::uint32_t>(width) / 16;
    auto const grid_size_y = static_cast<std::uint32_t>(height) / 16;

    std::cout << grid_size_x << 'x' << grid_size_y << '\n';

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

    std::uint32_t camera_ssbo_handle = 0;

    {
        app_state.camera.aspect = static_cast<float>(width) / static_cast<float>(height);

        glUseProgram(compute_program.handle);

        glCreateBuffers(1, &camera_ssbo_handle);
        glObjectLabel(GL_BUFFER, camera_ssbo_handle, -1, "[BO]");

        glNamedBufferStorage(camera_ssbo_handle, sizeof(scene::camera::gpu_data), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kCAMERA_BINDING, camera_ssbo_handle);

        glUseProgram(0);
    }

    {
        std::vector<primitives::sphere> spheres;

        spheres.emplace_back(primitives::sphere{glm::vec3{0, 0, -1}, .5f, 0});
        spheres.emplace_back(primitives::sphere{glm::vec3{0, -100.5f, -1}, 100.f, 3});
        
        std::uint32_t primitives_ssbo_handle = 0;

        glUseProgram(compute_program.handle);

        glCreateBuffers(1, &primitives_ssbo_handle);
        glObjectLabel(GL_BUFFER, primitives_ssbo_handle, -1, "[BO]");

        glNamedBufferStorage(primitives_ssbo_handle, sizeof(primitives::sphere) * std::size(spheres), std::data(spheres), GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kPRIMITIVES_BINDING, primitives_ssbo_handle);

        glUseProgram(0);
    }

    {
        std::random_device random_device;
        std::mt19937 generator{random_device()};

        std::vector<glm::vec3> buffer(kUNIT_VECTORS_BUFFER_SIZE);

        std::generate(std::execution::par_unseq, std::begin(buffer), std::end(buffer), [&generator]
        {
            return glm::normalize(math::random_in_unit_sphere(generator));
        });

        std::uint32_t ssbo_handle{0};

        glUseProgram(compute_program.handle);

        glCreateBuffers(1, &ssbo_handle);
        glObjectLabel(GL_BUFFER, ssbo_handle, -1, "[BO]");

        glNamedBufferStorage(ssbo_handle, sizeof(glm::vec3) * std::size(buffer), std::data(buffer), 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kUNIT_VECTORS_BUFFER_BINDING, ssbo_handle);

        glUseProgram(0);
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    window.update([&] (auto &&window)
    {
        glfwPollEvents();

        auto start = std::chrono::system_clock::now();

        app_state.camera.look_at(glm::vec3{0, 0, 0}, glm::vec3{0, 0, -1}, glm::vec3{0, 1, 0});
        app_state.camera.update();

        auto [app_width, app_height] = app_state.window_size;
        auto [fbo_width, fbo_height] = render_pass.framebuffer.size;

        glNamedBufferSubData(camera_ssbo_handle, 0, sizeof(scene::camera::gpu_data), &app_state.camera.data);

        glUseProgram(compute_program.handle);
        glDispatchCompute(grid_size_x, grid_size_y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(screen_quad_program.handle);
        render_scene(render_pass);

        glBlitNamedFramebuffer(render_pass.framebuffer.handle, 0, 0, 0, fbo_width, fbo_height, 0, 0, app_width, app_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window.handle());

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

        glfwSetWindowTitle(window.handle(), std::to_string(duration.count()).c_str());

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));
    });

    glfwTerminate();
}
