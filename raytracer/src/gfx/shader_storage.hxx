#pragma once

#include <cstdint>
#include <cstddef>

#include "gfx/context.hxx"


namespace gfx
{
    template<class T>
    struct shader_storage_buffer final {
        using type = T;

        std::uint32_t handle{ 0 };
        std::uint32_t binding_index{ 0 };
        std::uint32_t length{ 0 };
    };

    template<class T>
    gfx::shader_storage_buffer<T>
    create_shader_storage_buffer(std::uint32_t binding_index, std::uint32_t length, T const *data)
    {
        std::uint32_t handle;

        glCreateBuffers(1, &handle);
        glObjectLabel(GL_BUFFER, handle, -1, "[BO]");

        auto size_in_bytes = static_cast<std::ptrdiff_t>(sizeof(T) * length);

        glNamedBufferStorage(handle, size_in_bytes, data, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index, handle);

        return gfx::shader_storage_buffer<T>{ handle, binding_index, length };
    }

    template<class T>
    gfx::shader_storage_buffer<T> create_shader_storage_buffer(std::uint32_t binding_index, std::uint32_t length)
    {
        std::uint32_t handle;

        glCreateBuffers(1, &handle);
        glObjectLabel(GL_BUFFER, handle, -1, "[BO]");

        auto size_in_bytes = static_cast<std::ptrdiff_t>(sizeof(T) * length);

        glNamedBufferStorage(handle, size_in_bytes, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index, handle);

        return gfx::shader_storage_buffer<T>{ handle, binding_index, length };
    }

    template<class T>
    void update_shader_storage_buffer(gfx::shader_storage_buffer<T> const &buffer, T const *data)
    {
        auto size_in_bytes = static_cast<std::ptrdiff_t>(sizeof(T) * buffer.length);

        glNamedBufferSubData(buffer.handle, 0, size_in_bytes, data);
    }
}
