//
// Created by buranskyd on 10/5/16.
//
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "shader.h"
#include "mesh.h"

#include "gl_texture_vert.h"
#include "gl_texture_frag.h"

const unsigned int SIZE = 512;

struct Pixel {
    unsigned char r, g, b;
};

struct Point {
    int x,y;
};

Pixel framebuffer[SIZE][SIZE]; // premenna na naplnanie bufferu a potom ho vykresli

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
//    auto texture_id = LoadImage("result.rgb", SIZE, SIZE);
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<char *>(framebuffer) );
    auto texture_attrib = program->GetUniformLocation("Texture");
    glUniform1i(texture_attrib, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Main execution loop
    while (!glfwWindowShouldClose(window)) {
        // Set gray background
//        glClearColor(.5f,.5f,.5f,0);
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

void bresenham(Point &vertex, Point &point) {

    int x1=vertex.x;
    int y1=vertex.y;
    int x2=point.x;
    int y2=point.y;

    x2=SIZE / 2 - x2;
    y2=SIZE / 2 + y2;
    x1=SIZE / 2 - x1;
    y1=SIZE / 2 + y1;

    int delta_x=x2 - x1;
    signed char const ix=(delta_x > 0) - (delta_x < 0); //bud 0 alebo 1
    delta_x = std::abs(delta_x) << 1;

    int delta_y=y2 - y1;
    signed char const iy=(delta_y > 0) - (delta_y < 0);
    delta_y = std::abs(delta_y) << 1;


    if (delta_x >= delta_y) {           //vykreslovanie pod mensim uhlom
        // error may go below zero
        int error=delta_y - (delta_x >> 1);

        while (x1 != x2) {
            if ((error >= 0) && (error || (ix > 0))) {
                error -= delta_x;
                y1 += iy;
            }
            error += delta_y;
            x1 += ix;

            framebuffer[x1][y1].r = 255;
            framebuffer[x1][y1].g = 0;
            framebuffer[x1][y1].b = 255;
        }
    }
    else{                       //vykreslovanie pod vacsim uhlom
        int error(delta_x - (delta_y >> 1));

        while (y1 != y2){
            if ((error >= 0) && (error || (iy > 0))) {
                error -= delta_y;
                x1 += ix;
            }
            error += delta_x;
            y1 += iy;

            framebuffer[x1][y1].r = 255;
            framebuffer[x1][y1].g = 0;
            framebuffer[x1][y1].b = 255;
        }
    }
}

int main()
{
    std::vector<Point> vertex;

    double const countofVertices=5; //pocet vrcholov hviezdy
    double length = SIZE / 2;   //polomer
    double val = M_PI * 2 / countofVertices *2; // posuvam sa o 144 stupnov
    double x2, x1 = cos(0) * length;
    double y2, y1 = sin(0) * length;
    vertex.push_back(Point());
    vertex.push_back(Point());

    for(int i=1 ; i<=countofVertices ; i++){
        x2 = cos(i * val) * length;
        y2 = sin(i * val) * length;
        
        vertex[0].x=(int)x1;     //pridavanie suradnic do vectoru
        vertex[0].y=(int)y1;
        vertex[1].x=(int)x2;
        vertex[1].y=(int)y2;
        bresenham(vertex[0], vertex[1]);

        x1 =x2;
        y1 = y2;
    }

    createWindow(); // vytvorenie okna
    return EXIT_SUCCESS;
}