#pragma once

#include <cstdint>
#include <cstddef>

#include "gfx/context.hxx"
#include "gfx/framebuffer.hxx"

namespace gfx {
struct render_pass final {
    std::uint32_t vao;

    gfx::framebuffer framebuffer;
};

gfx::render_pass create_render_pass(gfx::framebuffer &&framebuffer)
{
    std::uint32_t vao;
    glCreateVertexArrays(1, &vao);
    glObjectLabel(GL_VERTEX_ARRAY, vao, -1, "[VAO]");

    return gfx::render_pass{vao, std::move(framebuffer)};
}
}
