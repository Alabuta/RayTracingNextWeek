﻿cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

project(RayTracingNextWeek VERSION 1.0.0 LANGUAGES CXX)

configure_file(
    "${PROJECT_SOURCE_DIR}/raytracer/include/config.hxx.in"
    "${PROJECT_SOURCE_DIR}/raytracer/include/config.hxx"
)

set(CMAKE_LINK_LIBRARY_USING_load_archive TRUE)
set(CMAKE_C_LINK_LIBRARY_USING_load_archive_SUPPORTED TRUE)
set(CMAKE_CXX_LINK_LIBRARY_USING_load_archive_SUPPORTED TRUE)

set(USE_ASAN OFF)

set(GLFW_LIBRARY_TYPE "SHARED" CACHE STRING "" FORCE)

include("cmake/InitSanitizers.cmake")
include("cmake/SetCompilerFlagsStyle.cmake")

find_package(OpenGL REQUIRED)
find_package(GLEW 2.1 REQUIRED)
find_package(Boost 1.68 COMPONENTS program_options REQUIRED)

set(SUBMODULES
    fmt
    glfw
    glm
)

foreach(SUBMODULE ${SUBMODULES})
    message(STATUS "Including submodule ${SUBMODULE}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/${SUBMODULE} EXCLUDE_FROM_ALL SYSTEM)
endforeach()

if(NOT WIN32)
    find_package(X11 REQUIRED)
endif()

set(EXECUTABLE_TARGET_NAME raytracer)
add_executable(${EXECUTABLE_TARGET_NAME})

target_include_directories(${EXECUTABLE_TARGET_NAME}
    SYSTEM PRIVATE
        OpenGL::GL
        ${Boost_INCLUDE_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/external
        ./raytracer/include

    PRIVATE
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
        ${EXTRA_COMPILER_OPTIONS}

        "$<$<OR:$<BOOL:${CXX_FLAGS_STYLE_GNU}>,$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>>:"
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

        "$<$<BOOL:${CXX_FLAGS_STYLE_GNU}>:"
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

target_link_directories(${EXECUTABLE_TARGET_NAME}
    PRIVATE
        ${EXTRA_LINK_DIRS}
)

target_link_libraries(${EXECUTABLE_TARGET_NAME}
    PRIVATE
        "$<$<IN_LIST:${CMAKE_SYSTEM_NAME}," Linux";"Darwin">:"
            stdc++fs
            # pthread
        ">"

        fmt::fmt-header-only
        glfw
        GLEW::GLEW
        glm::glm

        Boost::program_options

        OpenGL::GL
)

target_link_options(${EXECUTABLE_TARGET_NAME}
    PRIVATE
        ${EXTRA_LINK_OPTIONS}

        "$<$<IN_LIST:${CMAKE_SYSTEM_NAME}," Linux";"Darwin">:"
            # LINKER:SHELL:-z defs
            LINKER:-z,defs;                         # Detect and reject underlinking LINKER:SHELL:-z defs
            LINKER:-z,now;                          # Disable lazy binding
            LINKER:-z,relro                         # Read-only segments after relocation
        ">"

        "$<$<OR:$<BOOL:${CXX_FLAGS_STYLE_GNU}>,$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>>:"
            LINKER:-pie;
            LINKER:-no-undefined;                   # Report unresolved symbol references from regular object files
            LINKER:-no-allow-shlib-undefined;       # Disallows undefined symbols in shared libraries
            LINKER:-unresolved-symbols=report-all
        ">"
)
