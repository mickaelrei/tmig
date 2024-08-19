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

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Library output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Engine source files
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(
    ENGINE_SOURCE_FILES

    # Physics objects
    ${SOURCE_DIR}/physics/rigid-body.cpp

    # Render GL resources
    ${SOURCE_DIR}/render/gl/ebo.cpp
    ${SOURCE_DIR}/render/gl/shader.cpp
    ${SOURCE_DIR}/render/gl/texture_cube.cpp
    ${SOURCE_DIR}/render/gl/texture.cpp
    ${SOURCE_DIR}/render/gl/vao.cpp
    ${SOURCE_DIR}/render/gl/vbo.cpp

    # Render utils
    ${SOURCE_DIR}/render/utils/primitives_mesh.cpp
    ${SOURCE_DIR}/render/utils/primitives_gmesh.cpp
    ${SOURCE_DIR}/render/utils/shaders.cpp
    ${SOURCE_DIR}/render/utils/skybox.cpp

    # Render objects
    ${SOURCE_DIR}/render/camera.cpp
    ${SOURCE_DIR}/render/entity.cpp
    ${SOURCE_DIR}/render/gmesh.cpp
    ${SOURCE_DIR}/render/light.cpp
    ${SOURCE_DIR}/render/scene.cpp
    ${SOURCE_DIR}/render/window.cpp

    # Engine commons
    ${SOURCE_DIR}/init.cpp
    ${SOURCE_DIR}/mesh.cpp

    # External resources
    ${CMAKE_SOURCE_DIR}/external/glad/glad.c
    ${CMAKE_SOURCE_DIR}/external/stb/stb_image.c
)

# Adding engine shared library
add_library(${TMIG} SHARED ${ENGINE_SOURCE_FILES})

# Libraries
target_link_directories(
    ${TMIG}
    PRIVATE
    ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(
    ${TMIG}
    PRIVATE
    ${OPENGL_LIBRARIES} glfw
)

# Header files
target_include_directories(
    ${TMIG}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/include
)

# Tests
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/bin)

#######################################################
#######################################################
# Main scene test

# Source files
set(
    SCENE_TEST_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/tests/scene-test.cpp
)

# Adding executables for testing
add_executable(scene-test ${SCENE_TEST_SOURCE_FILES})

# Libraries
target_link_directories(
    scene-test
    PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
target_link_libraries(
    scene-test
    PRIVATE
    ${TMIG}
    ${OPENGL_LIBRARIES} glfw
)

# Header files
target_include_directories(
    scene-test
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

#######################################################
#######################################################
# Rigid Body Simulation

# Source files
set(
    RIGIDBODY_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/tests/rigid-body.cpp
)

# Adding executables for testing
add_executable(rigid-body ${RIGIDBODY_SOURCE_FILES})

# Libraries
target_link_directories(
    rigid-body
    PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
target_link_libraries(
    rigid-body
    PRIVATE
    ${TMIG}
    ${OPENGL_LIBRARIES} glfw
)

# Header files
target_include_directories(
    rigid-body
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
