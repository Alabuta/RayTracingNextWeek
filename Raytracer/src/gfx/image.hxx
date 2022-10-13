#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include "gfx/context.hxx"


namespace gfx
{
    struct image2D final {
        std::uint32_t handle{0};
        std::int32_t width{0}, height{0};
    };

    static gfx::image2D create_image2D(std::int32_t width, std::int32_t height, GLenum internal_format);

    gfx::image2D create_image2D(std::int32_t width, std::int32_t height, GLenum internal_format)
    {
        using namespace std::string_literals;
        std::uint32_t handle;

        glCreateTextures(GL_TEXTURE_2D, 1, &handle);
        glObjectLabel(GL_TEXTURE, handle, -1, "[texture object]");

        glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(handle, 1, internal_format, width, height);

        return { handle, width, height };
    }
}
