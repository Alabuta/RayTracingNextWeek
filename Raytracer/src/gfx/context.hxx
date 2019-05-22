#pragma once

#include <string>
using namespace std::string_literals;

//#define GL_GLEXT_PROTOTYPES 0

#include <GL/glew.h>
#include <GL/glcorearb.h>

//#ifdef _MSC_VER
//    #define WGL_WGLEXT_PROTOTYPES 0
//    #include <GL/wglext.h>
//
//#elif defined(__GNUG__)
//    #include <GL/glx.h>
//    #include <GL/glxext.h>
//#endif

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
        glfwSwapInterval(0);

        glewExperimental = true;

        glDisable(GL_CULL_FACE);

        if (auto result = glewInit(); result != GLEW_OK)
            throw std::runtime_error("failed to init GLEW"s);
    }
};
}
