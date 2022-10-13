cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

project(RayTracingNextWeek VERSION 1.0.0 LANGUAGES CXX)

configure_file(
    "./include/config.hxx.in"
    "./include/config.hxx"
)

set(USE_ASAN OFF)

# Get compiler info
set(CXX_FLAGS_STYLE_GNU OFF)
set(CXX_FLAGS_STYLE_MSVC OFF)
set(CXX_FLAGS_STYLE_CLANGCL OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CXX_FLAGS_STYLE_GNU ON)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if("${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
        set(CXX_FLAGS_STYLE_CLANGCL ON)
    else()
        set(CXX_FLAGS_STYLE_GNU ON)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CXX_FLAGS_STYLE_MSVC ON)
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

find_package(OpenGL REQUIRED)

find_package(glm REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW 2.1 REQUIRED)
find_package(fmt 5.3 CONFIG REQUIRED)
find_package(Boost 1.68 COMPONENTS program_options REQUIRED)
# find_package(STBImage REQUIRED)

if(NOT WIN32)
    find_package(X11 REQUIRED)
endif()

set(EXECUTABLE_TARGET_NAME ${PROJECT_NAME})
add_executable(${EXECUTABLE_TARGET_NAME})

include_directories(SYSTEM include "/usr/include/stb")
include_directories(src)

target_sources(${EXECUTABLE_TARGET_NAME}
    PRIVATE
        src/math/math.hxx					            src/math/math.cxx
        src/math/perlin.hxx                             src/math/perlin.cxx

        src/gfx/context.hxx
        src/gfx/framebuffer.hxx
        src/gfx/image.hxx
        src/gfx/render_pass.hxx
        src/gfx/shader.hxx
        src/gfx/shader_storage.hxx
        src/gfx/atomic_counter.hxx
        src/gfx/vertex_array.hxx

        src/loaders/image_loader.hxx                    src/loaders/image_loader.cxx
        src/loaders/spirv_loader.hxx                    src/loaders/spirv_loader.cxx

        src/camera/camera.hxx                           src/camera/camera.cxx
        src/camera/camera_controller.hxx                src/camera/camera_controller.cxx
        src/camera/input_handlers/mouse_handler.hxx     src/camera/input_handlers/mouse_handler.cxx
        src/camera/input_handlers/keyboard_handler.hxx  src/camera/input_handlers/keyboard_handler.cxx

        src/raytracer/bvh.hxx
        src/raytracer/material.hxx
        src/raytracer/physics.hxx
        src/raytracer/primitives.hxx
        src/raytracer/texture.hxx

        src/input/input_manager.hxx                     src/input/input_manager.cxx
        src/input/mouse.hxx								src/input/mouse.cxx
        src/input/keyboard.hxx							src/input/keyboard.cxx

        src/platform/platform.hxx
        src/platform/window.hxx		                    src/platform/window.cxx

        src/main.hxx					                src/main.cxx
)

if(NOT WIN32)
    set(EXTRA_LIBS ${EXTRA_LIBS}
        stdc++fs
        # pthread
    )

    set(EXTRA_LIBS_LINK_OPTIONS ${EXTRA_LIBS_LINK_OPTIONS}
        -Wl,-no-undefined
        -Wl,-no-allow-shlib-undefined
        -Wl,-unresolved-symbols=report-all
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}

    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED NO
    CXX_EXTENSIONS OFF

    POSITION_INDEPENDENT_CODE ON

    DEBUG_POSTFIX .d
)

#[[target_compile_options(${PROJECT_NAME}
    PRIVATE
        ## GCC/Clang Flags
        $<$<BOOL:${CXX_FLAGS_STYLE_GNU}>:"-someflag">

        ## MSVC flags
        $<$<BOOL:${CXX_FLAGS_STYLE_MSVC}>:"/someflag">

        ## CLANG-CL flags
        $<$<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>:"-someflag">
)]]

if (${USE_ASAN})
    set(EXTRA_LIBS ${EXTRA_LIBS}
        clang_rt.asan_dynamic-x86_64
        clang_rt.asan_dynamic_runtime_thunk-x86_64
    )

    set(EXTRA_COMPILER_OPTIONS ${EXTRA_COMPILER_OPTIONS}
        -fsanitize=address
    )

    set(EXTRA_LINK_OPTIONS ${EXTRA_LINK_OPTIONS}
        /wholearchive:clang_rt.asan_dynamic_runtime_thunk-x86_64.lib
    )

    set(EXTRA_LINK_DIRS ${EXTRA_LINK_DIRS}
        "C:/Program Files/LLVM/lib/clang/15.0.2/lib/windows"
    )
endif ()


if (($<BOOL:${CXX_FLAGS_STYLE_GNU}> OR $<BOOL:${CXX_FLAGS_STYLE_CLANGCL}>))
    target_link_options(${EXECUTABLE_TARGET_NAME}
        PRIVATE
            ${EXTRA_LINK_OPTIONS}

            # LINKER: -pie
            # LINKER: -z,defs                             # Detect and reject underlinking
            # LINKER: -z,now                              # Disable lazy binding
            # LINKER: -z,relro                            # Read-only segments after relocation
            # LINKER: -no-undefined                       # Report unresolved symbol references from regular object files
            # LINKER: -no-allow-shlib-undefined           # Disallows undefined symbols in shared libraries
            LINKER: -unresolved-symbols=report-all
    )
endif ()

if(${CXX_FLAGS_STYLE_GNU})
    target_compile_options (${EXECUTABLE_TARGET_NAME}
        PRIVATE
            ${EXTRA_COMPILER_OPTIONS}

            -fconcepts

            -fasynchronous-unwind-tables                # Increased reliability of backtraces
            -fexceptions                                # Enable table-based thread cancellation
            # -fPIE
            # -fpie

            -pipe

            -Wpedantic
            -Wall
            -Wextra
            -Werror
            -Wold-style-cast
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
            -Wmisleading-indentation
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast

            -Wno-unknown-pragmas

            -g
        )
endif()

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND ${CXX_FLAGS_STYLE_CLANGCL}))
    target_compile_options (${EXECUTABLE_TARGET_NAME}
        PRIVATE
            ${EXTRA_COMPILER_OPTIONS}

            -Wpedantic
            -Wall
            /EHa
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-covered-switch-default
            -Wno-undef
            -Wno-switch-enum
            -Wextra
            -Werror
            -Wold-style-cast
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
            -Wconversion
            -Woverloaded-virtual
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wno-shadow-field-in-constructor
            -Wformat=2
            # -Wlifetime

            -Wno-unknown-pragmas
            -Wno-unknown-warning-option

            -Wno-nonportable-system-include-path
    )
endif()

if(${CXX_FLAGS_STYLE_MSVC})
    target_compile_options (${EXECUTABLE_TARGET_NAME}
        PRIVATE
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
        )
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        OpenGL::GL
)

target_link_directories (${EXECUTABLE_TARGET_NAME}
    PRIVATE
        ${EXTRA_LINK_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${EXTRA_LIBS_LINK_OPTIONS}
        ${EXTRA_LIBS}

        glfw
        glm::glm
        GLEW::GLEW

        fmt::fmt-header-only
        #fmt::fmt

        Boost::program_options

        OpenGL::GL
)
