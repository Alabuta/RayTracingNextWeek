#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <variant>

#include <string>
using namespace std::string_literals;

#include <string_view>
using namespace std::string_view_literals;

#include "loaders/spirv_loader.hxx"
#include "gfx/context.hxx"


namespace gfx::shader {
enum class STAGE {
    VERTEX = 0x01,
    TESS_CONTROL = 0x02,
    TESS_EVAL = 0x04,
    GEOMETRY = 0x08,
    FRAGMENT = 0x10,
    COMPUTE = 0x20
};

template<STAGE S> struct stage {
    static auto constexpr semantic{S};

    std::uint32_t handle;

    std::string module_name;
    std::string entry_point;

    virtual ~stage() = default;
};

struct vertex final : public stage<STAGE::VERTEX> { };
struct tess_control final : public stage<STAGE::TESS_CONTROL> { };
struct tess_eval final : public stage<STAGE::TESS_EVAL> { };
struct geometry final : public stage<STAGE::GEOMETRY> { };
struct fragment final : public stage<STAGE::FRAGMENT> { };
struct compute final : public stage<STAGE::COMPUTE> { };
}

namespace gfx {
using shader_stage = std::variant<
    shader::vertex,
    shader::tess_control,
    shader::tess_eval,
    shader::geometry,
    shader::fragment,
    shader::compute
>;

template<class T>
gfx::shader_stage create_shader_stage(std::string_view module_name, std::string_view entry_point)
{
    auto byte_code = loader::load_spirv(module_name);

    if (byte_code.empty())
        throw std::runtime_error("failed to load shader byte code"s);

    T shader_stage;

    shader_stage.module_name = module_name;
    shader_stage.entry_point = entry_point;

    switch (T::semantic) {
        case gfx::shader::STAGE::VERTEX:
            shader_stage.handle = glCreateShader(GL_VERTEX_SHADER);
            break;

        case gfx::shader::STAGE::FRAGMENT:
            shader_stage.handle = glCreateShader(GL_FRAGMENT_SHADER);
            break;

        case gfx::shader::STAGE::COMPUTE:
            shader_stage.handle = glCreateShader(GL_COMPUTE_SHADER);
            break;

        default:
            throw std::runtime_error("unsupported shader stage"s);
    }

    glObjectLabel(GL_SHADER, shader_stage.handle, -1, "[shader stage object]");

    glShaderBinary(1, &shader_stage.handle, GL_SHADER_BINARY_FORMAT_SPIR_V,
                   std::data(byte_code), static_cast<std::int32_t>(std::size(byte_code)));

    std::string _entry_point{entry_point};
    glSpecializeShader(shader_stage.handle, _entry_point.c_str(), 0, nullptr, nullptr);

    auto status = 0;
    glGetShaderiv(shader_stage.handle, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        auto length = -1;
        glGetShaderiv(shader_stage.handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            std::vector<char> log(static_cast<std::size_t>(length), '\0');
            glGetShaderInfoLog(shader_stage.handle, static_cast<std::int32_t>(log.size()), &length, std::data(log));

            std::string error_log{std::begin(log), std::end(log)};

            throw std::runtime_error("shader stage compilation error: "s + error_log);
        }
    }

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    return shader_stage;
}
}

namespace gfx {
struct program final {
    std::uint32_t handle;

    std::vector<gfx::shader_stage> shader_stages;
};

gfx::program create_program(std::vector<gfx::shader_stage> const &shader_stages)
{
    auto handle = glCreateProgram();

    glObjectLabel(GL_PROGRAM, handle, -1, "[shader program object]");

    for (auto &&shader_stage : shader_stages) {
        std::visit([handle] (auto &&stage)
        {
            glAttachShader(handle, stage.handle);

        }, shader_stage);
    }

    glLinkProgram(handle);

    auto status = 0;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        auto length = -1;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            std::vector<char> log(static_cast<std::size_t>(length), '\0');

            glGetProgramInfoLog(handle, static_cast<std::int32_t>(log.size()), &length, std::data(log));

            std::string error_log{std::begin(log), std::end(log)};

            throw std::runtime_error("shader program link error: "s + error_log);
        }
    }

    glValidateProgram(handle);

    glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

    if (status != GL_TRUE) {
        auto length = -1;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            std::vector<char> log(static_cast<std::size_t>(length), '\0');

            glGetProgramInfoLog(handle, static_cast<std::int32_t>(log.size()), &length, std::data(log));

            std::string error_log{std::begin(log), std::end(log)};

            throw std::runtime_error("shader program link error: "s + error_log);
        }
    }

    for (auto &&shader_stage : shader_stages) {
        std::visit([handle] (auto &&stage)
        {
            glDetachShader(handle, stage.handle);

        }, shader_stage);
    }

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("OpenGL error: "s + std::to_string(result));

    gfx::program program{handle, {std::cbegin(shader_stages), std::cend(shader_stages)}};

    return program;
}
}
