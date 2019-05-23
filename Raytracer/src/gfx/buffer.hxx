#pragma once

#include <cstdint>
#include <cstddef>

#include "gfx/context.hxx"


namespace gfx {
template<class T>
struct buffer final {
    using type = T;

    std::uint32_t handle{0};
    std::uint32_t binding_index{0};
    std::uint32_t length{0};
};

template<class T>
gfx::buffer<T> create_buffer(std::uint32_t binding_index, std::uint32_t length)
{
    std::uint32_t handle;

    glCreateBuffers(1, &handle);
    glObjectLabel(GL_BUFFER, handle, -1, "[BO]");

    auto size_in_bytes = static_cast<std::ptrdiff_t>(sizeof(T) * length);

    glNamedBufferData(handle, size_in_bytes, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index, handle);

    return gfx::buffer<T>{handle, binding_index, length};
}

template<class T>
void update_buffer(gfx::buffer<T> const &buffer, T const *data)
{
    auto size_in_bytes = static_cast<std::ptrdiff_t>(sizeof(T) * buffer.length);

    glNamedBufferSubData(buffer.handle, 0, size_in_bytes, data);
}
}
