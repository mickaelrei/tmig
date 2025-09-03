cmake_minimum_required(VERSION 2.8.5...3.27.5)

set(TMIG tmig)
project(${TMIG})

# Use Release mode by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Build type
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -g3 -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Library output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
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

# Debug option
option(DEBUG "Enable debug mode" OFF)
if (DEBUG)
    message("DEBUG MODE ON")
    target_compile_definitions(${TMIG} PUBLIC DEBUG)
else()
    message("DEBUG MODE OFF")
endif()

#######################################################
# ASSIMP SETUP
#######################################################
find_package(assimp QUIET)
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

    # Store current engine build type before changing it to force Assimp to build in Release
    set(PROJECT_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE Release)

    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors for Assimp")
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable unit tests for Assimp")
    add_subdirectory(external/assimp)

    # Restore your project's original build type
    set(CMAKE_BUILD_TYPE ${PROJECT_BUILD_TYPE})
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

message(STATUS "Compiling ImGui from source (forced Release mode)...")

# Store your project's build type and force ImGui to be configured in Release mode
set(PROJECT_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Release)

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

# Restore previous build type
set(CMAKE_BUILD_TYPE ${PROJECT_BUILD_TYPE})

# Enable Position Independent Code for the static library
set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
