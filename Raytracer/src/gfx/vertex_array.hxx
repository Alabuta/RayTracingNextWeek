#pragma once

#include <cstdint>
#include <cstddef>

#include <string>
using namespace std::string_literals;

#define GLM_FORCE_CXX17
#include <glm/glm.hpp>

#include "gfx/context.hxx"
#include "gfx/buffer.hxx"


namespace gfx {
template<std::size_t N, class T>
struct vertex_array final {
    static auto constexpr number{N};
    using type = T;

    std::uint32_t handle{0};
    std::uint32_t attribute_index{0};

    gfx::buffer<glm::vec<N, T>> buffer;
};

template<std::size_t N, class T>
gfx::vertex_array<N, T> create_vertex_array(std::uint32_t attribute_index, gfx::buffer<glm::vec<N, T>> const &buffer)
{
    std::uint32_t handle{0};

    glCreateVertexArrays(1, &handle);
    glObjectLabel(GL_VERTEX_ARRAY, handle, -1, "[VAO]");

    auto type = 0u;

    if constexpr (std::is_same_v<T, float>)
        type = GL_FLOAT;

    else throw std::runtime_error("unsupported attribute type"s);

    glVertexArrayAttribBinding(handle, attribute_index, 0);
    glVertexArrayAttribFormat(handle, attribute_index, N, type, GL_FALSE, 0);
    glVertexArrayVertexBuffer(handle, 0, buffer.handle, 0, sizeof(glm::vec<N, T>));
    glEnableVertexArrayAttrib(handle, attribute_index);

    return gfx::vertex_array<N, T>{handle, attribute_index, buffer};
}
}
