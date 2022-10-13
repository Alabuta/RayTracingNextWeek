#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>

#include "gfx/context.hxx"
#include "gfx/image.hxx"


namespace gfx
{
    struct framebuffer final {
        std::uint32_t handle{ 0 };

        std::array<std::int32_t, 2> size{ 0, 0 };

        gfx::image2D attachment;
    };

    static gfx::framebuffer create_framebuffer(std::int32_t width, std::int32_t height, gfx::image2D attachment)
    {
        using namespace std::string_literals;

        std::uint32_t handle;

        glCreateFramebuffers(1, &handle);
        glObjectLabel(GL_FRAMEBUFFER, handle, -1, "[framebuffer object]");

        glNamedFramebufferTexture(handle, GL_COLOR_ATTACHMENT0, attachment.handle, 0);

        std::array<std::uint32_t, 1> constexpr drawBuffers{ GL_COLOR_ATTACHMENT0 };
        glNamedFramebufferDrawBuffers(handle, static_cast<std::int32_t>(std::size(drawBuffers)), std::data(drawBuffers));

        if (auto result = glCheckNamedFramebufferStatus(handle, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("framebuffer: "s + std::to_string(result));

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));

        return gfx::framebuffer{ handle, std::array{ width, height }, attachment };
    }
}
