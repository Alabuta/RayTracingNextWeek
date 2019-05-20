#pragma once

//#define GL_GLEXT_PROTOTYPES 0

#include <GL/glew.h>
#include <GL/glcorearb.h>

#ifdef _MSC_VER
    //#define WGL_WGLEXT_PROTOTYPES 0
    //#include <GL/wglext.h>

#elif defined(__GNUG__)
    #include <GL/glx.h>
    #include <GL/glxext.h>
#endif

#ifdef _MSC_VER
    #pragma comment(lib, "OpenGL32.lib")
    #pragma comment(lib, "glew32d.lib")
    #pragma comment(lib, "glfw3dll.lib")
#endif

#include "platform/window.hxx"


namespace gfx {
struct context final {
    GLFWwindow *handle{nullptr};

    context(platform::window const &window) : handle{window.handle()}
    {
        glfwMakeContextCurrent(handle);
        glfwSwapInterval(-1);

        glewExperimental = true;

        if (auto result = glewInit(); result != GLEW_OK)
            throw std::runtime_error("failed to init GLEW"s);
    }
};


struct framebuffer final {
    std::array<std::int32_t, 2> size{0, 0};

    std::uint32_t handle{0u};
    std::uint32_t attachment_handle{0u};

    framebuffer(std::int32_t width, std::int32_t height) : size{std::array{width, height}}
    {
        glCreateFramebuffers(1, &handle);
        glObjectLabel(GL_FRAMEBUFFER, handle, -1, "[framebuffer object]");

        glCreateTextures(GL_TEXTURE_2D, 1, &attachment_handle);
        glObjectLabel(GL_TEXTURE, attachment_handle, -1, "[texture object]");

        glTextureParameteri(attachment_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(attachment_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(attachment_handle, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(attachment_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(attachment_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(attachment_handle, 1, GL_SRGB8_ALPHA8, width, height);

        glNamedFramebufferTexture(handle, GL_COLOR_ATTACHMENT0, attachment_handle, 0);

        std::array<std::uint32_t, 1> constexpr drawBuffers{GL_COLOR_ATTACHMENT0};
        glNamedFramebufferDrawBuffers(handle, static_cast<std::int32_t>(std::size(drawBuffers)), std::data(drawBuffers));

        if (auto result = glCheckNamedFramebufferStatus(handle, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("framebuffer: "s + std::to_string(result));

        if (auto result = glGetError(); result != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "s + std::to_string(result));
    }

    ~framebuffer()
    {
        glDeleteTextures(1, std::data(std::array{attachment_handle}));
        glDeleteFramebuffers(1, std::data(std::array{handle}));
    }
};
}
