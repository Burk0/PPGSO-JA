# This CMake script is designed for CLion IDE and QTCreator projects on Windows
# It should find bundled binary dependencies automatically when using MinGW compiler
# On Linux and Mac the dependencies (glm, glew, glfw) need to be installed manually

cmake_minimum_required(VERSION 3.1)
project(ppgso CXX)

#
# CONFIGURATION
#

# Basic CMake settings
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Add custom target for installation in CLion

add_custom_target(run_install COMMAND ${CMAKE_MAKE_PROGRAM} install)

# Use generate_shader function to convert .glsl sources to C++ headers
include(shaders)

#
# DEPENDENCIES
#

# Set up external dependencies for Windows users
if (MINGW)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/include/")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/mingw")
elseif (MSVC)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/include/")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/vc2015")
    set(STRICT_COMPILE_FLAGS)
endif ()

# Warnings for Debug mode
option(USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON)
# These compile flags should apply for GCC and Clang compilers
set(STRICT_COMPILE_FLAGS "-Wpedantic -Wall -Wno-c++98-compat -Wfloat-equal -Wextra -Wsign-promo -Wsign-compare -Wconversion -Wno-sign-conversion -Wno-unused-parameter")
if (USE_STRICT_COMPILE_WARNINGS)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${STRICT_COMPILE_FLAGS}")
endif ()

# Find required packages
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)

# Set default installation destination
if (NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "../_install")
endif ()

# PPGSO library
add_library(libppgso STATIC
        src/lib/mesh.cpp
        src/lib/tiny_obj_loader.cpp
        src/lib/shader.cpp
        src/lib/texture.cpp)
# Make sure GLM uses radians and static GLEW library
target_compile_definitions(libppgso PUBLIC -DGLM_FORCE_RADIANS -DGLEW_STATIC )
# Link to GLFW, GLEW and OpenGL
target_link_libraries(libppgso PUBLIC ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
# Pass on include directories
target_include_directories(libppgso PUBLIC
        src/lib
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

#
# TARGETS
#

# u1
set(U1_SRC src/u1/u1.cpp)
generate_shaders(GL_TEXTURE_SHADERS
        src/u1/u1_frag.glsl
        src/u1/u1_vert.glsl)
add_executable(u1 ${U1_SRC})
target_link_libraries(u1 libppgso)
install (TARGETS u1 DESTINATION .)

# u2
set(U2_SRC src/u2/u2.cpp)
generate_shaders(GL_TEXTURE_SHADERS
        src/u2/u2_frag.glsl
        src/u2/u2_vert.glsl)
add_executable(u2 ${U2_SRC})
target_link_libraries(u2 libppgso)
install (TARGETS u2 DESTINATION .)

# Triangle
set(TRIANGLE_SRC src/Triangle/Triangle.cpp src/u2/u2.cpp)
generate_shaders(TRIANGLE_SHADERS
        src/Triangle/Triangle_frag.glsl
        src/Triangle/Triangle_vert.glsl)
add_executable(Triangle ${TRIANGLE_SRC})
target_link_libraries(Triangle libppgso)
install (TARGETS Triangle DESTINATION .)



#
# INSTALLATION
#

install(DIRECTORY data/ DESTINATION src/Triangle)
