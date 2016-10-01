// Example raw_gradient
// - Illustrates the concept of a framebuffer
// - We do not really need any libraries or hardware to do computer graphics
// - In this case the framebuffer is simply saved as a raw RGB and TGA image

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "mesh.h"

#include "gl_texture_vert.h"
#include "gl_texture_frag.h"

// Size of the framebuffer
const unsigned int SIZE = 512;
// Load a new image from a raw RGB file directly into OpenGL memory

GLuint LoadImage(const std::string &image_file, unsigned int width, unsigned int height) {
    // Create new texture object
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Read raw data
    std::ifstream image_stream(image_file, std::ios::binary);

    // Setup buffer for pixels (r,g,b bytes), since we will not manipulate the image just keep it as char
    std::vector<char> buffer(width*height*3);
    image_stream.read(buffer.data(), buffer.size());
    image_stream.close();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

    return texture_id;
}

void createWindow()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(1);
    }

    // Setup OpenGL context
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Try to create a window
    auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_texture", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1"
                  << std::endl;
        glfwTerminate();
        exit(1);
    }

    // Finalize window setup
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    if (!glewIsSupported("GL_VERSION_3_3")) {
        std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
        glfwTerminate();
        exit(1);
    }
    // Load shaders
    auto program = ShaderPtr(new Shader{gl_texture_vert, gl_texture_frag});
    program->Use();

    auto quad = Mesh{program, "quad.obj"};

    // Load and bind texture
    auto texture_id = LoadImage("result.rgb", SIZE, SIZE);
    auto texture_attrib = program->GetUniformLocation("Texture");
    glUniform1i(texture_attrib, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Main execution loop
    while (!glfwWindowShouldClose(window)) {
        // Set gray background
        glClearColor(.5f,.5f,.5f,0);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        quad.Render();

        // Display result
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Clean up
    glfwTerminate();

}
// A simple RGB struct will represent a pixel in the framebuffer
// Note: unsigned char is range <0, 255>
//       signed char is range <-128, 127>
struct Pixel {
    unsigned char r, g, b;
};

int main() {
    // Initialize a framebuffer
    // NOTE: The framebuffer is allocated on the stack. For bigger sizes this should be changed to a heap allocation.
    Pixel framebuffer[SIZE][SIZE];

    // Example: Generate a simple gradient
    for (unsigned int x = 0; x < SIZE; ++x) {
        for (unsigned int y = 0; y < SIZE; ++y) {
            framebuffer[x][y].r = 0;
            framebuffer[x][y].g = 0;
            framebuffer[x][y].b = 0;
        }
    }

    // Task1: Load RAW image file here instead
    FILE *f;
    f = std::fopen("lena.rgb", "r");
    if (!f) {
        std::perror("FILE NEOTVORENY");
        return EXIT_FAILURE;
    }
    fread(framebuffer, 1, sizeof(framebuffer), f);
    for (unsigned int x = 0; x < SIZE; ++x) {
        for (unsigned int y = 0; y < SIZE; ++y) {
            framebuffer[x][y].r = 255 - framebuffer[x][y].r;
            framebuffer[x][y].g = 255 - framebuffer[x][y].g;
            framebuffer[x][y].b = 255 - framebuffer[x][y].b;
        }
    }
    // Save the raw image to a file
    std::cout << "Generating result.rgb file ..." << std::endl;
    std::ofstream raw("result.rgb", std::ios::binary);
    raw.write(reinterpret_cast<char *>(framebuffer), sizeof(framebuffer));
    raw.close();

    std::cout << "Done." << std::endl;
    createWindow();
    return EXIT_SUCCESS;
}