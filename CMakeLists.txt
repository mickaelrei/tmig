cmake_minimum_required(VERSION 2.8.5...3.27.5)

set(TMIG tmig)
project(${TMIG})

# Warning flags
set(FLAGS "-Wall -Wextra")
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Build type
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAGS} -g3 -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FLAGS} -O3")

# Debug option
option(DEBUG "Enable debug mode" OFF)
if (DEBUG)
    message("DEBUG MODE ON")
    add_definitions(-DDEBUG)
else()
    message("DEBUG MODE OFF")
endif()

# Library output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Git QUIET)

#######################################################
# IMGUI SETUP
#######################################################
# Check if the ImGui submodule source is present
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/external/imgui/imgui.h")
    message(STATUS "ImGui source not found. Attempting to pull submodule...")
    if(GIT_FOUND)
        # Pull submodules recursively
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --progress
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMODULE_RESULT
        )
        if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FATAL_ERROR "Git submodule update failed. Please run 'git submodule update --init --recursive' manually.")
        endif()
    else()
        message(FATAL_ERROR "ImGui source not found and Git is not installed. Please run 'git submodule update --init --recursive' manually or install Git.")
    endif()
endif()

# Create a dedicated library for ImGui
set(IMGUI imgui)
set(
    IMGUI_SOURCE_FILES

    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(${IMGUI} SHARED ${IMGUI_SOURCE_FILES})
target_include_directories(
    ${IMGUI}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

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

# Libraries
target_link_libraries(
    ${TMIG}
    PUBLIC
        ${IMGUI}
        ${OPENGL_LIBRARIES}
        glfw
)

# Header files
target_include_directories(
    ${TMIG}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/include
)

#######################################################
# HELPER FUNCTION FOR CREATING TEST EXECUTABLES
#######################################################
function(add_engine_test TEST_NAME)
    # This function creates an executable from a source file in the /tests directory
    # It assumes the source file is named <TEST_NAME>.cpp

    add_executable(
        ${TEST_NAME}
        ${CMAKE_SOURCE_DIR}/tests/${TEST_NAME}.cpp
    )

    # Link the executable ONLY to our engine library
    # The engine library (tmig) will provide all other dependencies (GLFW, OpenGL, ImGui)
    target_link_libraries(
        ${TEST_NAME}
        PRIVATE
        ${TMIG}
    )
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
