cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

if(NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION 1.0.0)
endif()

project(RayTracingNextWeek VERSION ${PROJECT_VERSION})

enable_language(CXX)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/config.h"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")

find_package(OpenGL REQUIRED)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

if(NOT WIN32)
    find_package(X11 REQUIRED)
endif()

include_directories(SYSTEM Raytracer/include)
include_directories(Raytracer/src)

set(SOURCE_FILES
    Raytracer/src/gfx/context.hxx

    Raytracer/src/platform/platform.hxx
    Raytracer/src/platform/window.hxx		Raytracer/src/platform/window.cxx

	Raytracer/src/main.hxx					Raytracer/src/main.cxx
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}

    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED NO
    CXX_EXTENSIONS OFF

    POSITION_INDEPENDENT_CODE ON

    DEBUG_POSTFIX .d
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE
        -Wpedantic
        -Wall
        -Wextra
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Wcast-align
        -Wunused
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        # -Wlifetime

        #-fsanitize=thread -fsanitize=address
)

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wpedantic
        -Wall
        -Wextra
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Wcast-align
        -Wunused
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2

        #-fsanitize=thread -fsanitize=address
)

elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3
)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    OpenGL::GL
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    #-Wl,-no-undefined
    #-Wl,-no-allow-shlib-undefined
    #-Wl,-unresolved-symbols=report-all

    #pthread

    #stdc++fs

    glm
    glfw
    GLEW::GLEW

    OpenGL::GL
)
