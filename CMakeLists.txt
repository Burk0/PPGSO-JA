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
set(U1_SRC src/u1/u1.cpp src/u6/u6.cpp)
generate_shaders(u1_SHADERS
        src/u1/u1_frag.glsl
        src/u1/u1_vert.glsl)
add_executable(u1 ${U1_SRC} ${u1_SHADERS})
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

## Triangle
#set(TRIANGLE_SRC src/Triangle/Triangle.cpp src/Triangle/Triangle.cpp)
#generate_shaders(TRIANGLE_SHADERS
#        src/Triangle/Triangle_frag.glsl
#        src/Triangle/Triangle_vert.glsl)
#add_executable(Triangle ${TRIANGLE_SRC})
#target_link_libraries(Triangle libppgso)
#install (TARGETS Triangle DESTINATION .)
#

# u3
set(u3_SRC src/u3/u3.cpp src/u3/u3.cpp)
generate_shaders(u3_SHADERS
        src/u3/u3_vert.glsl
        src/u3/u3_frag.glsl)
add_executable(u3 ${u3_SRC} ${u3_SHADERS})
target_link_libraries(u3 libppgso)
install (TARGETS u3 DESTINATION .)

# u4
set(u4_SRC src/u4/u4.cpp src/u4/u4.cpp)
generate_shaders(u4_SHADERS
        src/u4/u4_vert.glsl
        src/u4/u4_frag.glsl)
add_executable(u4 ${u4_SRC} ${u4_SHADERS})
target_link_libraries(u4 libppgso)
install (TARGETS u4 DESTINATION .)


# u5
set(u5_SRC src/u5/u5.cpp src/u5/u5.cpp)
generate_shaders(u5_SHADERS
        src/u5/u5_vert.glsl
        src/u5/u5_frag.glsl)
add_executable(u5 ${u5_SRC} ${u5_SHADERS})
target_link_libraries(u5 libppgso)
install (TARGETS u5 DESTINATION .)

# u6
set(u6_SRC src/u6/u6.cpp src/u6/u6.cpp)
generate_shaders(u6_SHADERS
        src/u6/u6_vert.glsl
        src/u6/u6_frag.glsl)
add_executable(u6 ${u6_SRC} ${u6_SHADERS})
target_link_libraries(u6 libppgso)
install (TARGETS u6 DESTINATION .)

# u7
set(u7_SRC src/u7/u7.cpp)
generate_shaders(u7_SHADERS
        src/u7/gl_projection_vert.glsl
        src/u7/gl_projection_frag.glsl)
add_executable(u7 ${u7_SRC} ${u7_SHADERS})
target_link_libraries(u7 libppgso)
install(TARGETS u7 DESTINATION .)

# gl_scene
set(GL_SCENE_SRC
        src/gl_scene/gl_scene.cpp
        src/gl_scene/object.cpp
        src/gl_scene/scene.cpp
        src/gl_scene/camera.cpp
        src/gl_scene/block.cpp
        src/gl_scene/player.cpp
        src/gl_scene/space.cpp
        src/gl_scene/mapka.cpp
        src/gl_scene/lavaBlock.cpp
        src/gl_scene/kuzel.cpp
        src/gl_scene/finishBlock.cpp)
generate_shaders(GL_SCENE_SHADERS
        src/gl_scene/space_frag.glsl
        src/gl_scene/space_vert.glsl
        src/gl_scene/object_frag.glsl
        src/gl_scene/object_vert.glsl)
add_executable(gl_scene ${GL_SCENE_SRC} ${GL_SCENE_SHADERS})
target_link_libraries(gl_scene libppgso)
install(TARGETS gl_scene DESTINATION .)


#
# INSTALLATION
#

install(DIRECTORY data/ DESTINATION .)
