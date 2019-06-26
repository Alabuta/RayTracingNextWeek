#include "main.hxx"

#include "gfx/context.hxx"
#include "platform/window.hxx"

#include "input/input_manager.hxx"

#include "loaders/image_loader.hxx"

#include "math/perlin.hxx"

#include "gfx/framebuffer.hxx"
#include "gfx/render_pass.hxx"
#include "gfx/shader.hxx"
#include "gfx/image.hxx"
#include "gfx/shader_storage.hxx"
#include "gfx/vertex_array.hxx"

#include "camera/camera.hxx"
#include "camera/camera_controller.hxx"

#include "raytracer/bvh.hxx"
#include "raytracer/material.hxx"
#include "raytracer/physics.hxx"
#include "raytracer/primitives.hxx"


auto constexpr kVERTEX_SEMANTIC_POSITION = 0u;

auto constexpr kBVH_TREE_BINDING = 1u;
auto constexpr kOUT_IMAGE_BINDING = 2u;
auto constexpr kLAMBERTIAN_BUFFER_BINDING = 2u;
auto constexpr kMETAL_BUFFER_BINDING = 3u;
auto constexpr kDIELECTRIC_BUFFER_BINDING = 4u;
auto constexpr kEMISSIVE_BUFFER_BINDING = 5u;
auto constexpr kPRIMITIVES_BINDING = 6u;
auto constexpr kCAMERA_BINDING = 7u;
auto constexpr kPERLIN_NOISE_BINDING = 8u;


auto constexpr kDEBUG_SPHERICAL_FIBONACCI_LATTICE = false;
auto constexpr kUNIT_VECTORS_NUMBER = 8'192u;
auto constexpr kUNIT_VECTORS_BUFFER_BINDING = 5u;

auto constexpr kGROUP_SIZE = glm::uvec2{8, 8};


namespace app {
struct state final {
    std::array<std::int32_t, 2> window_size{0, 0};

    scene::camera_system camera_system;
    std::shared_ptr<scene::camera> camera;

    std::unique_ptr<camera::orbit_controller> camera_controller;

    gfx::shader_storage_buffer<scene::camera::gpu_data> camera_buffer;

    gfx::render_pass render_pass;

    gfx::program compute_program;
    gfx::program blit_program;

    gfx::program sphere_debug_program;
    gfx::vertex_array<3, float> sphere_debug_vao;

    gfx::shader_storage_buffer<primitives::sphere> spheres_buffer;
    std::vector<primitives::sphere> spheres;

    // std::unique_ptr<physics::system> physics_system;
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

void update(app::state &app_state, [[maybe_unused]] float delta_time)
{
    app_state.camera_controller->update();
    app_state.camera_system.update();

    gfx::update_shader_storage_buffer(app_state.camera_buffer, &app_state.camera->data);

    // app_state.physics_system->update(delta_time);
    // gfx::update_shader_storage_buffer(app_state.spheres_buffer, std::data(app_state.spheres));
}

void render(app::state const &app_state, glm::uvec2 groups_number)
{
    glUseProgram(app_state.compute_program.handle);

    glDispatchCompute(groups_number.x, groups_number.y, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void render_spherical_fibonacci_lattice(app::state const &app_state)
{
    glUseProgram(app_state.sphere_debug_program.handle);

    auto [width, height] = app_state.window_size;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(app_state.sphere_debug_vao.handle);
    glDrawArrays(GL_POINTS, 0, kUNIT_VECTORS_NUMBER);
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

    app_state.window_size = std::array{800, 600};
    auto [width, height] = app_state.window_size;

    auto const groups_number = glm::uvec2{glm::ceil(glm::vec2{width, height} / glm::vec2{kGROUP_SIZE})};

    std::cout << groups_number.x << 'x' << groups_number.y << '\n';

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
        app_state.camera = app_state.camera_system.create_camera(42.f, aspect);

        app_state.camera_controller = std::make_unique<camera::orbit_controller>(app_state.camera, *input_manager);
        app_state.camera_controller->look_at(glm::vec3{6.4, 1.6, 2.7}, glm::vec3{0, 1, 0});

        app_state.camera_buffer = gfx::create_shader_storage_buffer<scene::camera::gpu_data>(kCAMERA_BINDING, 1);

        gfx::update_shader_storage_buffer(app_state.camera_buffer, &app_state.camera->data);
    }

    {
        {
            std::vector<material::lambertian> lambertian;

            lambertian.push_back({glm::vec3{.2, .4, .5}});
            lambertian.push_back({glm::vec3{.6, .8, .8}});

            auto length = static_cast<std::uint32_t>(std::size(lambertian));

            gfx::create_shader_storage_buffer<material::lambertian>(kLAMBERTIAN_BUFFER_BINDING, length, std::data(lambertian));
        }

        {
            std::vector<material::metal> metal;

            metal.push_back({glm::vec3{.8, .6, .2}, 0});

            auto length = static_cast<std::uint32_t>(std::size(metal));

            gfx::create_shader_storage_buffer<material::metal>(kMETAL_BUFFER_BINDING, length, std::data(metal));
        }

        {
            std::vector<material::dielectric> dielectric;

            dielectric.push_back({glm::vec3{1}, 1.5f});

            auto length = static_cast<std::uint32_t>(std::size(dielectric));

            gfx::create_shader_storage_buffer<material::dielectric>(kDIELECTRIC_BUFFER_BINDING, length, std::data(dielectric));
        }

        {
            std::vector<material::emissive> emissive;

            emissive.push_back({glm::vec3{1}, 4.f});

            auto length = static_cast<std::uint32_t>(std::size(emissive));

            gfx::create_shader_storage_buffer<material::emissive>(kEMISSIVE_BUFFER_BINDING, length, std::data(emissive));
        }
    }

    {
        auto &&spheres = app_state.spheres;

        spheres.emplace_back(primitives::sphere{glm::vec3{0, 1, 0}, 1, 3, 0});
        spheres.emplace_back(primitives::sphere{glm::vec3{0, -1000, 0}, 1000, 0, 1});

        spheres.emplace_back(primitives::sphere{glm::vec3{+2.1, 1, 0}, 1, 1, 0});

        spheres.emplace_back(primitives::sphere{glm::vec3{-2.1, 1, 0}, 1, 2, 0});
        spheres.emplace_back(primitives::sphere{glm::vec3{-2.1, 1, 0}, -.98f, 2, 0});

        spheres.emplace_back(primitives::sphere{glm::vec3{0, 1, 2}, 0.2f, 2, 0});

        auto length = static_cast<std::uint32_t>(std::size(spheres));

        app_state.spheres_buffer = gfx::create_shader_storage_buffer<primitives::sphere>(kPRIMITIVES_BINDING, length);
        gfx::update_shader_storage_buffer(app_state.spheres_buffer, std::data(app_state.spheres));
    }

    // app_state.physics_system = std::make_unique<physics::system>(app_state.spheres);

    if constexpr (false) {
        auto unit_vectors_image = gfx::create_image2D(width, height, GL_RGBA32F);

        std::vector<glm::vec4> unit_vectors(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

        std::random_device random_device;
        std::mt19937 generator{random_device()};

    #ifdef _MSC_VER
        std::generate(std::execution::par_unseq, std::begin(unit_vectors), std::end(unit_vectors), [&generator]
    #else
        std::generate(std::begin(unit_vectors), std::end(unit_vectors), [&generator]
    #endif
        {
            return glm::vec4(glm::normalize(math::random_on_unit_sphere(generator)), 1);
        });

        glTextureSubImage2D(unit_vectors_image.handle, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, std::data(unit_vectors));
        glBindImageTexture(kUNIT_VECTORS_BUFFER_BINDING, unit_vectors_image.handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    }

    if constexpr (false) {
#if 1
        auto unit_vectors = math::spherical_fibonacci_lattice(kUNIT_VECTORS_NUMBER);
#else
        std::vector<glm::vec3> unit_vectors(kUNIT_VECTORS_NUMBER);

        std::random_device random_device;
        std::mt19937 generator{random_device()};

    #ifdef _MSC_VER
        std::generate(std::execution::par_unseq, std::begin(unit_vectors), std::end(unit_vectors), [&generator]
    #else
        std::generate(std::begin(unit_vectors), std::end(unit_vectors), [&generator]
    #endif
        {
            return glm::normalize(math::random_on_unit_sphere(generator));
        });
#endif

        auto buffer = gfx::create_shader_storage_buffer<glm::vec3>(kUNIT_VECTORS_BUFFER_BINDING, kUNIT_VECTORS_NUMBER, std::data(unit_vectors));

        if constexpr (kDEBUG_SPHERICAL_FIBONACCI_LATTICE) {
            auto vertex_stage = gfx::create_shader_stage<gfx::shader::vertex>("debug-sphere.vert.spv"sv, "main"sv);
            auto fragment_stage = gfx::create_shader_stage<gfx::shader::fragment>("debug-sphere.frag.spv"sv, "main"sv);

            app_state.sphere_debug_program = gfx::create_program(std::vector{vertex_stage, fragment_stage});
            app_state.sphere_debug_vao = gfx::create_vertex_array<3, float>(kVERTEX_SEMANTIC_POSITION, buffer);
        }
    }

    {
        auto perlin = math::create_perlin_noise();

        gfx::create_shader_storage_buffer<math::perlin>(kPERLIN_NOISE_BINDING, 1, &perlin);
    }

    auto texture_image = loader::load_image("earth-sphere.jpg"sv);
    glBindTextureUnit(12, texture_image.handle);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glFinish();

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    auto last_time = std::chrono::high_resolution_clock::now();

    window.update([&] (auto &&window)
    {
        auto start = std::chrono::high_resolution_clock::now();

        auto delta_time = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(start - last_time).count()) * 1e-6f;

        last_time = start;

        glfwPollEvents();

        app::update(app_state, delta_time);

        if constexpr (kDEBUG_SPHERICAL_FIBONACCI_LATTICE) {
            app::render_spherical_fibonacci_lattice(app_state);
        }

        else {
            app::render(app_state, groups_number);

            glFinish();

            app::blit_framebuffer(app_state);
        }

        glfwSwapBuffers(window.handle());

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));

        glfwSetWindowTitle(window.handle(), ("Raytracer "s + std::to_string(delta_time * 1e+3f)).c_str());
    });

    glfwTerminate();
}
