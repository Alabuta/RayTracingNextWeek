#include "main.hxx"

#include "gfx/context.hxx"
#include "platform/window.hxx"

#include "input/input_manager.hxx"

#include "gfx/framebuffer.hxx"
#include "gfx/render_pass.hxx"
#include "gfx/shader.hxx"
#include "gfx/image.hxx"
#include "gfx/buffer.hxx"

#include "camera/camera.hxx"
#include "camera/camera_controller.hxx"

#include "raytracer/primitives.hxx"


auto constexpr kOUT_IMAGE_BINDING = 2u;
auto constexpr kUNIT_VECTORS_BUFFER_BINDING = 5u;
auto constexpr kPRIMITIVES_BINDING = 6u;
auto constexpr kCAMERA_BINDING = 7u;

auto constexpr kFRAME_NUMBER_UNIFORM_LOCATION = 1u;


namespace app {
struct state final {
    std::array<std::int32_t, 2> window_size{0, 0};

    scene::camera_system camera_system;
    std::shared_ptr<scene::camera> camera;

    std::unique_ptr<camera::orbit_controller> camera_controller;

    gfx::buffer<scene::camera::gpu_data> camera_buffer;

    gfx::render_pass render_pass;

    gfx::program compute_program;
    gfx::program blit_program;
};

class window_event_handler final : public platform::event_handler {
public:

    window_event_handler(app::state &app_state) noexcept : app_state{app_state} { }

    void on_resize(std::int32_t width, std::int32_t height) override
    {
        if (width * height == 0)
            return;

        app_state.window_size = std::array{width, height};

        app_state.camera->aspect = static_cast<float>(width) / static_cast<float>(height);
    }

private:
    app::state &app_state;
};

void update(app::state &app_state)
{
    app_state.camera_controller->update();
    app_state.camera_system.update();

    gfx::update_buffer(app_state.camera_buffer, &app_state.camera->data);
}

void render(app::state const &app_state, std::uint32_t grid_size_x, std::uint32_t grid_size_y)
{
    glUseProgram(app_state.compute_program.handle);

    glDispatchCompute(grid_size_x, grid_size_y, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void blit_framebuffer(app::state const &app_state)
{
    glUseProgram(app_state.blit_program.handle);

    auto &&[vao, framebuffer] = app_state.render_pass;

    auto [app_width, app_height] = app_state.window_size;
    auto [fbo_width, fbo_height] = framebuffer.size;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.handle);

    glViewport(0, 0, fbo_width, fbo_height);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBlitNamedFramebuffer(framebuffer.handle, 0, 0, 0, fbo_width, fbo_height,
                           0, 0, app_width, app_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
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

    app::state app_state;

    app_state.window_size = std::array{1920, 1080};
    auto [width, height] = app_state.window_size;

    auto const grid_size_x = static_cast<std::uint32_t>(std::ceil(width / 8.f));
    auto const grid_size_y = static_cast<std::uint32_t>(std::ceil(height / 8.f));

    std::cout << grid_size_x << 'x' << grid_size_y << '\n';

    platform::window window{"Raytracer"sv, width, height};

    auto app_window_event_handler = std::make_shared<app::window_event_handler>(app_state);
    window.connect_handler(app_window_event_handler);

    auto input_manager = std::make_shared<input::input_manager>();
    window.connect_handler(input_manager);

    gfx::context context{window};

    auto image = gfx::create_image2D(width, height, GL_RGBA32F);

    glBindImageTexture(kOUT_IMAGE_BINDING, image.handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTextureUnit(4, image.handle);

    {
        auto attachment = gfx::create_image2D(width, height, GL_SRGB8_ALPHA8);
        auto framebuffer = gfx::create_framebuffer(width, height, attachment);

        app_state.render_pass = gfx::create_render_pass(std::move(framebuffer));
    }

    {
        auto compute_shader_stage = gfx::create_shader_stage<gfx::shader::compute>("shader.comp.spv"sv, "main"sv);
        app_state.compute_program = gfx::create_program(std::vector{compute_shader_stage});
    }

    {
        auto vertex_stage = gfx::create_shader_stage<gfx::shader::vertex>("shader.vert.spv"sv, "main"sv);
        auto fragment_stage = gfx::create_shader_stage<gfx::shader::fragment>("shader.frag.spv"sv, "main"sv);

        app_state.blit_program = gfx::create_program(std::vector{vertex_stage, fragment_stage});
    }

    {
        auto aspect = static_cast<float>(width) / static_cast<float>(height);
        app_state.camera = app_state.camera_system.create_camera(72.f, aspect);

        app_state.camera_controller = std::make_unique<camera::orbit_controller>(app_state.camera, *input_manager);
        app_state.camera_controller->look_at(glm::vec3{0, 2, 4}, glm::vec3{0, .5f, 0});

        app_state.camera_buffer = gfx::create_buffer<scene::camera::gpu_data>(kCAMERA_BINDING, 1);

        gfx::update_buffer(app_state.camera_buffer, &app_state.camera->data);
    }

    {
        std::vector<primitives::sphere> spheres;

        spheres.emplace_back(primitives::sphere{glm::vec3{0, 0, -1}, .5f, 0});
        spheres.emplace_back(primitives::sphere{glm::vec3{0, -100.5f, -1}, 100.f, 3});

        auto buffer = gfx::create_buffer<primitives::sphere>(kPRIMITIVES_BINDING, static_cast<std::uint32_t>(std::size(spheres)));

        gfx::update_buffer(buffer, std::data(spheres));
    }

    if (false) {
        auto unit_vectors_image = gfx::create_image2D(width, height, GL_RGBA32F);

        std::random_device random_device;
        std::mt19937 generator{random_device()};

        std::vector<glm::vec4> unit_vectors(static_cast<std::size_t>(width * height));

    #ifdef _MSC_VER
        std::generate(std::execution::par_unseq, std::begin(unit_vectors), std::end(unit_vectors), [&generator]
        {
            return glm::vec4(glm::normalize(math::random_on_unit_sphere(generator)), 1);
        });
    #else
        std::generate(std::begin(unit_vectors), std::end(unit_vectors), [&generator]
        {
            return glm::vec4(glm::normalize(math::random_on_unit_sphere(generator)), 1);
        });
    #endif

        glTextureSubImage2D(unit_vectors_image.handle, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, std::data(unit_vectors));
        glBindImageTexture(kUNIT_VECTORS_BUFFER_BINDING, unit_vectors_image.handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    window.update([&] (auto &&window)
    {
        glfwPollEvents();

        app::update(app_state);

        auto start = std::chrono::high_resolution_clock::now();

        app::render(app_state, grid_size_x, grid_size_y);

        glFinish();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

        glfwSetWindowTitle(window.handle(), std::to_string(static_cast<float>(duration.count()) * 1e-3).c_str());

        app::blit_framebuffer(app_state);

        glfwSwapBuffers(window.handle());

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));
    });

    glfwTerminate();
}
