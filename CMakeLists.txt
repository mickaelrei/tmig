cmake_minimum_required(VERSION 2.8.5...3.27.5)

set(PROJECT_NAME tmig)

project(${PROJECT_NAME})

# Warning flags
set(FLAGS "-Wall -Wextra -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

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
    ${SOURCE_DIR}/gl/ebo.cpp
    ${SOURCE_DIR}/gl/shader.cpp
    ${SOURCE_DIR}/gl/texture.cpp
    ${SOURCE_DIR}/gl/vao.cpp
    ${SOURCE_DIR}/gl/vbo.cpp

    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/entity.cpp
    ${SOURCE_DIR}/mesh.cpp
    ${SOURCE_DIR}/window.cpp


    ${CMAKE_SOURCE_DIR}/external/glad/glad.c
    ${CMAKE_SOURCE_DIR}/external/stb/stb_image.c
)

# Adding engine shared library
add_library(${PROJECT_NAME} SHARED ${ENGINE_SOURCE_FILES})

# Libraries
target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${OPENGL_LIBRARIES} glfw
)

# Header files
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/include
)


# Tests
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/bin)

# Source files
set(
    TEST_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
)

# Adding executables for testing
add_executable(tests ${TEST_SOURCE_FILES})

# Libraries
target_link_directories(
    tests
    PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
target_link_libraries(
    tests
    PRIVATE
    ${PROJECT_NAME}
    ${OPENGL_LIBRARIES} glfw
)

# Header files
target_include_directories(
    tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
