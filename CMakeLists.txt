cmake_minimum_required(VERSION 2.8.5...3.27.5)

set(TMIG tmig)
project(${TMIG})

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Library output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp QUIET)
find_package(Git QUIET)

#######################################################
# ENGINE SETUP
#######################################################
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(
    ENGINE_SOURCE_FILES

    # Render module
    ${SOURCE_DIR}/render/postprocessing/bloom.cpp
    ${SOURCE_DIR}/render/postprocessing/blur.cpp
    ${SOURCE_DIR}/render/camera.cpp
    ${SOURCE_DIR}/render/framebuffer.cpp
    ${SOURCE_DIR}/render/render.cpp
    ${SOURCE_DIR}/render/shader.cpp
    ${SOURCE_DIR}/render/texture2D.cpp
    ${SOURCE_DIR}/render/ui.cpp
    ${SOURCE_DIR}/render/vertex_attribute.cpp
    ${SOURCE_DIR}/render/window.cpp

    # Core module
    ${SOURCE_DIR}/core/callback_manager.cpp
    ${SOURCE_DIR}/core/input.cpp
    ${SOURCE_DIR}/core/light_manager.cpp

    # Utility module
    ${SOURCE_DIR}/util/camera_controller.cpp
    ${SOURCE_DIR}/util/file.cpp
    ${SOURCE_DIR}/util/postprocessing.cpp
    ${SOURCE_DIR}/util/resources.cpp
    ${SOURCE_DIR}/util/shapes.cpp
    ${SOURCE_DIR}/util/time_step.cpp

    # External resources
    ${CMAKE_SOURCE_DIR}/external/glad/glad.c
    ${CMAKE_SOURCE_DIR}/external/stb/stb_image.c
)

# Adding engine shared library
add_library(${TMIG} SHARED ${ENGINE_SOURCE_FILES})
target_compile_definitions(${TMIG} PUBLIC PROJECT_ROOT_FOLDER="${CMAKE_SOURCE_DIR}")
target_compile_options(${TMIG} PRIVATE
    -Wall
    -Wextra
)

# Header files
target_include_directories(
    ${TMIG}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/include
)

#######################################################
# ENGINE CONFIGURATION
#######################################################

# Base engine flags
target_compile_definitions(${TMIG} PRIVATE PROJECT_ROOT_FOLDER="${CMAKE_SOURCE_DIR}")
target_compile_options(${TMIG} PRIVATE -Wall -Wextra)

# Debug option
option(DEBUG "Enable debug mode" OFF)
if (DEBUG)
    message("DEBUG MODE ON")
    target_compile_definitions(${TMIG} PUBLIC DEBUG)
else()
    message("DEBUG MODE OFF")
endif()

# Engine-only config (never reconfigures the whole project)
set(ENGINE_CONFIG "Release" CACHE STRING "Engine-only build config")
set_property(CACHE ENGINE_CONFIG PROPERTY STRINGS "Debug;RelWithDebInfo;Release")

# Apply engine-only variants
if (ENGINE_CONFIG STREQUAL "Debug")
    # Debug flags
    target_compile_definitions(${TMIG} PRIVATE DEBUG)
    target_compile_options(${TMIG} PRIVATE -g3 -fno-omit-frame-pointer)
    target_link_options(${TMIG} PRIVATE -fsanitize=address,undefined)
    target_compile_options(${TMIG} PRIVATE -fsanitize=address,undefined)

    # Propagate to test executables that link the engine
    target_link_options(${TMIG} INTERFACE -fsanitize=address,undefined)
elseif (ENGINE_CONFIG STREQUAL "RelWithDebInfo")
    # Release with debug info flags
    target_compile_definitions(${TMIG} PRIVATE NDEBUG)
    target_compile_options(${TMIG} PRIVATE -O2 -g)
else()
    # Release flags
    target_compile_definitions(${TMIG} PRIVATE NDEBUG)
    target_compile_options(${TMIG} PRIVATE -O3)
endif()

#######################################################
# ASSIMP SETUP
#######################################################
if (assimp_FOUND)
    # If find_package was successful, use the imported system library
    message(STATUS "Found system-installed Assimp. Using it.")
    set(ASSIMP_LIBRARY assimp::assimp)
else()
    message(STATUS "System-installed Assimp not found. Checking for source files...")

    # Check if the submodule source is present
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/external/assimp/CMakeLists.txt")
        message(STATUS "Assimp source not found. Attempting to pull submodule...")
        if(GIT_FOUND)
            # Pull submodules recursively
            execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --progress external/assimp
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMODULE_RESULT
            )
            if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
                message(FATAL_ERROR "Git submodule update failed. Please run manually:\ngit submodule update --init --recursive --progress external/assimp")
            endif()
        else()
            message(FATAL_ERROR "Assimp source not found and Git is not installed. Please run manually:\ngit submodule update --init --recursive --progress external/assimp\nOr install Git.")
        endif()
    endif()

    # Lock Assimp build to Release regardless of ENGINE_CONFIG
    set(PROJECT_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)

    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Disable tools" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "Don't install Assimp" FORCE)

    add_subdirectory(external/assimp)

    # Restore the original project build type
    set(CMAKE_BUILD_TYPE ${PROJECT_BUILD_TYPE} CACHE STRING "" FORCE)

    set(ASSIMP_LIBRARY assimp)
endif()

#######################################################
# IMGUI SETUP
#######################################################
# Check if the ImGui submodule source is present
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/external/imgui/imgui.h")
    message(STATUS "ImGui source not found. Attempting to pull submodule...")
    if(GIT_FOUND)
        # Pull submodules recursively
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --progress external/imgui
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMODULE_RESULT
        )
        if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FATAL_ERROR "Git submodule update failed. Please run manually:\ngit submodule update --init --recursive --progress external/imgui")
        endif()
    else()
        message(FATAL_ERROR "ImGui source not found and Git is not installed. Please run manually:\ngit submodule update --init --recursive --progress external/imgui\nOr install Git.")
    endif()
endif()

message(STATUS "Compiling ImGui from source...")

# Create a dedicated library for ImGui
set(IMGUI imgui)
add_library(${IMGUI} STATIC
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)

# Enable Position Independent Code for the static library
target_compile_definitions(${IMGUI} PRIVATE NDEBUG)
target_compile_options(${IMGUI} PRIVATE -O3)
set_target_properties(${IMGUI} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Set the public include directories for the target
target_include_directories(${IMGUI} PUBLIC
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

##########################
# Set libraries for engine
target_link_libraries(
    ${TMIG}
    PUBLIC
        ${IMGUI}
        ${ASSIMP_LIBRARY}
        ${OPENGL_LIBRARIES}
        glfw
)

#######################################################
# HELPER FUNCTION FOR CREATING TEST EXECUTABLES
#######################################################
function(add_engine_test TEST_NAME)
    # This function creates an executable from a source file in the /tests directory
    # It assumes the source file is named <TEST_NAME>.cpp

    add_executable(${TEST_NAME} ${CMAKE_SOURCE_DIR}/tests/${TEST_NAME}.cpp)

    # Link the executable ONLY to our engine library
    # The engine library (tmig) will provide all other dependencies (GLFW, OpenGL, ImGui)
    target_link_libraries(${TEST_NAME} PRIVATE ${TMIG})
endfunction()

#######################################################
# TESTS
#######################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/bin)

# Use our new function to create all the test executables
add_engine_test(instanced)
add_engine_test(non_instanced)
add_engine_test(framebuffer)
add_engine_test(bloom)
add_engine_test(lights)
add_engine_test(assimp_test)
