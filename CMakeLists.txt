cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
include(FetchContent)

project(RayTracingNextWeek VERSION 1.0.0 LANGUAGES CXX)

configure_file(
    "${PROJECT_SOURCE_DIR}/raytracer/include/config.hxx.in"
    "${PROJECT_SOURCE_DIR}/raytracer/include/config.hxx"
)

message(CMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN_FILE}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

if(WIN32 AND MINGW)
    set(CXX_FLAGS_STYLE_MINGW_WINDOWS TRUE)
endif()

find_package(OpenGL REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS program_options)
find_package(GLEW 2.2 REQUIRED)

if(NOT WIN32)
    find_package(X11 REQUIRED)
endif()


# === GLM === (header-only)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.2
        SYSTEM
)

# === GLFW ===
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        SYSTEM
)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
        SYSTEM
)

FetchContent_MakeAvailable(glm glfw stb)


set(EXECUTABLE_TARGET_NAME raytracer)
add_executable(${EXECUTABLE_TARGET_NAME})

target_include_directories(${EXECUTABLE_TARGET_NAME}
    SYSTEM PRIVATE
        OpenGL::GL
        ${stb_SOURCE_DIR}
        ${Boost_INCLUDE_DIR}

    PRIVATE
        ./raytracer/include
        ./raytracer/src
)

target_sources(${EXECUTABLE_TARGET_NAME}
    PRIVATE
        ./raytracer/src/math/math.hxx                                   ./raytracer/src/math/math.cxx
        ./raytracer/src/math/perlin.hxx                                 ./raytracer/src/math/perlin.cxx

        ./raytracer/src/gfx/context.hxx
        ./raytracer/src/gfx/framebuffer.hxx
        ./raytracer/src/gfx/image.hxx
        ./raytracer/src/gfx/render_pass.hxx
        ./raytracer/src/gfx/shader.hxx
        ./raytracer/src/gfx/shader_storage.hxx
        ./raytracer/src/gfx/atomic_counter.hxx
        ./raytracer/src/gfx/vertex_array.hxx

        ./raytracer/src/loaders/image_loader.hxx                        ./raytracer/src/loaders/image_loader.cxx
        ./raytracer/src/loaders/spirv_loader.hxx                        ./raytracer/src/loaders/spirv_loader.cxx

        ./raytracer/src/camera/camera.hxx                               ./raytracer/src/camera/camera.cxx
        ./raytracer/src/camera/camera_controller.hxx                    ./raytracer/src/camera/camera_controller.cxx
        ./raytracer/src/camera/input_handlers/mouse_handler.hxx         ./raytracer/src/camera/input_handlers/mouse_handler.cxx
        ./raytracer/src/camera/input_handlers/keyboard_handler.hxx      ./raytracer/src/camera/input_handlers/keyboard_handler.cxx

        ./raytracer/src/raytracer/bvh.hxx
        ./raytracer/src/raytracer/material.hxx
        ./raytracer/src/raytracer/physics.hxx
        ./raytracer/src/raytracer/primitives.hxx
        ./raytracer/src/raytracer/texture.hxx

        ./raytracer/src/input/input_manager.hxx                         ./raytracer/src/input/input_manager.cxx
        ./raytracer/src/input/mouse.hxx                                 ./raytracer/src/input/mouse.cxx
        ./raytracer/src/input/keyboard.hxx                              ./raytracer/src/input/keyboard.cxx

        ./raytracer/src/platform/platform.hxx
        ./raytracer/src/platform/window.hxx                             ./raytracer/src/platform/window.cxx

        ./raytracer/src/main.hxx                                        ./raytracer/src/main.cxx
)

set_target_properties(${EXECUTABLE_TARGET_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}

        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF

        POSITION_INDEPENDENT_CODE ON

        DEBUG_POSTFIX .d
)

target_compile_options(${EXECUTABLE_TARGET_NAME}
    PRIVATE

        "$<$<OR:$<BOOL:${CXX_FLAGS_STYLE_GNU}>,$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>,$<BOOL:${CXX_FLAGS_STYLE_MINGW_WINDOWS}>>:"
            -Wpedantic
            -Wall
            -Wextra
            -Werror
            -Wconversion

            -Wold-style-cast
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast
            -Wmisleading-indentation

            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic

            -Wno-pre-c++17-compat
        ">"

        "$<$<OR:$<BOOL:${CXX_FLAGS_STYLE_GNU}>,$<BOOL:${CXX_FLAGS_STYLE_MINGW_WINDOWS}>>:"
            -fconcepts

            -fasynchronous-unwind-tables                # Increased reliability of backtraces
            -fexceptions                                # Enable table-based thread cancellation
            # -fPIE
            # -fpie

            -pipe
        ">"

        "$<$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>:"
            /EHa
            # -fcxx-exceptions

            -Wno-unknown-pragmas
            -Wno-unknown-warning-option

            -Wno-shadow-field-in-constructor
        ">"

        "$<$<BOOL:${CXX_FLAGS_STYLE_MSVC}>:"
            "/std:c++latest"

            # /W4
            # /WX
            /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss of data
            /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
            /w14263 # 'function': member function does not override any base class virtual member function
            /w14265 # 'classname': class has virtual functions, but destructor is not virtual
            /w14287 # 'operator': unsigned/negative constant mismatch
            /we4289 # 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
            /w14296 # 'operator': expression is always 'boolean_value'
            /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
            /w14545 # expression before comma evaluates to a function which is missing an argument list
            /w14546 # function call before comma missing argument list
            /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
            /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
            /w14555 # expression has no effect; expected expression with side-effect
            /w14619 # pragma warning: there is no warning number 'number'
            /w14640 # Enable warning on thread un-safe static member initialization
            /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
            /w14905 # wide string literal cast to 'LPSTR'
            /w14906 # string literal cast to 'LPWSTR'
            /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
        ">"
)

target_link_libraries(${EXECUTABLE_TARGET_NAME}
        PRIVATE

        "$<$<OR:$<BOOL:${CXX_FLAGS_STYLE_GNU}>,$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>,$<BOOL:${CXX_FLAGS_STYLE_MINGW_WINDOWS}>>:"
            stdc++fs
            stdc++exp
            # pthread
        ">"

        OpenGL::GL
        Boost::program_options

        glfw
        GLEW::GLEW
        glm::glm
)

target_link_options(${EXECUTABLE_TARGET_NAME}
    PRIVATE
        "$<$<IN_LIST:${CMAKE_SYSTEM_NAME}," Linux";"Darwin">:"
            # LINKER:SHELL:-z defs
            LINKER:-z,defs;                         # Detect and reject underlinking LINKER:SHELL:-z defs
            LINKER:-z,now;                          # Disable lazy binding
            LINKER:-z,relro                         # Read-only segments after relocation
        ">"

        "$<$<OR:$<BOOL:${CXX_FLAGS_STYLE_GNU}>,$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>,$<BOOL:${CXX_FLAGS_STYLE_MINGW_WINDOWS}>>:"
            LINKER:-pie;
            LINKER:-no-undefined;                   # Report unresolved symbol references from regular object files
            LINKER:-no-allow-shlib-undefined;       # Disallows undefined symbols in shared libraries
            LINKER:-unresolved-symbols=report-all
        ">"
)
