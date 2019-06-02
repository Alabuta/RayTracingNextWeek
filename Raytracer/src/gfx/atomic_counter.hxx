#pragma once

#include <cstdint>
#include <cstddef>

#include "gfx/context.hxx"


namespace gfx {
template<class T>
struct atomic_counter_buffer final {
    using type = T;

    std::uint32_t handle{0};
    std::uint32_t binding_index{0};
};

template<class T>
gfx::atomic_counter_buffer<T> create_atomic_counter_buffer(std::uint32_t binding_index)
{
    std::uint32_t handle;

    glCreateBuffers(1, &handle);
    glObjectLabel(GL_BUFFER, handle, -1, "[BO]");

    glNamedBufferStorage(handle, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, binding_index, handle);

    return gfx::atomic_counter_buffer<T>{handle, binding_index};
}

template<class T>
void update_atomic_counter_buffer(gfx::atomic_counter_buffer<T> const &buffer, T data)
{
    auto internal_format = 0u;
    auto data_format = 0u;
    auto data_type = 0u;

    if constexpr (std::is_same_v<T, std::uint32_t>) {
        internal_format = GL_R32UI;
        data_format = GL_RED;
        data_type = GL_UNSIGNED_INT;
    }

    else throw std::runtime_error("unsupported type"s);

    glClearNamedBufferSubData(buffer.handle, internal_format, 0, sizeof(T), data_format, data_type , &data);
}
}
