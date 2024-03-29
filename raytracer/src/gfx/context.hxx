#pragma once

#include <string>

//#define GL_GLEXT_PROTOTYPES 0

#if defined(__GCC__) || defined(__GNU__)
    #include <GL/glew.h>
    #include <GL/glcorearb.h>
#elif defined(_MSC_VER)
    #include <gl/glew.h>
    #include <gl/glcorearb.h>
#endif

//#ifdef _MSC_VER
//    #define WGL_WGLEXT_PROTOTYPES 0
//    #include <GL/wglext.h>
//
//#elif defined(__GNUG__)
//    #include <GL/glx.h>
//    #include <GL/glxext.h>
//#endif

/*#ifdef _MSC_VER
    #pragma comment(lib, "OpenGL32.lib")
    #ifdef _DEBUG
        #pragma comment(lib, "glew32d.lib")
    #else
        #pragma comment(lib, "glew32.lib")
    #endif
    #pragma comment(lib, "glfw3dll.lib")
#endif*/

#include "platform/window.hxx"


namespace gfx {
struct context final {
    GLFWwindow *handle{nullptr};

    context(platform::window const &window) : handle{window.handle()}
    {
        using namespace std::string_literals;

        glfwMakeContextCurrent(handle);
        glfwSwapInterval(0);

        glewExperimental = true;

        if (auto result = glewInit(); result != GLEW_OK)
            throw std::runtime_error("failed to init GLEW"s);

        glPointSize(2.f);

        glClearColor(0.f, .254901f, .6f, 1.f);

        glDisable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_CLAMP);
        glDepthFunc(GL_LESS);

        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    }
};
}
