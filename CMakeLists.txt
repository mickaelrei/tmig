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

    # Render module
    ${SOURCE_DIR}/render/camera.cpp
    ${SOURCE_DIR}/render/render.cpp
    ${SOURCE_DIR}/render/shader.cpp
    ${SOURCE_DIR}/render/window.cpp

    # Utility module
    ${SOURCE_DIR}/util/camera.cpp
    ${SOURCE_DIR}/util/debug.cpp
    ${SOURCE_DIR}/util/resources.cpp
    ${SOURCE_DIR}/util/shapes.cpp

    # External resources
    ${CMAKE_SOURCE_DIR}/external/glad/glad.c
    ${CMAKE_SOURCE_DIR}/external/stb/stb_image.c
)

# Adding engine shared library
add_library(${TMIG} SHARED ${ENGINE_SOURCE_FILES})
target_compile_definitions(${TMIG} PUBLIC PROJECT_ROOT_FOLDER="${CMAKE_SOURCE_DIR}")

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
# Main test

# Source files
set(
    MAIN_TEST_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
)

# Adding executables for testing
add_executable(main ${MAIN_TEST_SOURCE_FILES})

# Libraries
target_link_directories(
    main
    PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
target_link_libraries(
    main
    PRIVATE
    ${TMIG}
    ${OPENGL_LIBRARIES} glfw
)

# Header files
target_include_directories(
    main
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/include
)