// Example gl_gradient
// - Demonstrates the use of a dynamically generated gradient in GLSL
// - Displays the gradient on a quad using OpenGL

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "u5_vert.h"
#include "u5_frag.h"

const unsigned int SIZE = 512;

void SetTransformation(ShaderPtr program, float time) {
  // Create transformation matrix
  // NOTE: glm matrices are declared column major !

  // identity
  auto transform = glm::mat3({
                                     1.0, 0.0, 0.0,
                                     0.0, 1.0, 0.0,
                                     0.0, 0.0, 1.0
                             });

  // scale
//   transform *= glm::mat3({
//      std::sin(time), 0.0, 0.0,
//      0.0, std::sin(time), 0.0,
//      0.0, 0.0, 1.0
//    });

  // squash
  // transform *= glm::mat3({
  //   std::sin(time), 0.0, 0.0,
  //   0.0, std::cos(time), 0.0,
  //   0.0, 0.0, 1.0
  // });

    // translate
    transform *= glm::mat3({
                                   1.0, 0.0, 0.0,
                                   0.0, 1.0, 0.0,
                                   std::sin(time), -std::cos(time), 1.0,
                           });
  // rotate
  transform *= glm::mat3({
                                 std::cos(time), std::sin(time), 0.0,
                                 -std::sin(time), std::cos(time), 0.0,
                                 0.0, 0.0, 1.0
                         });




  // Rotate around top right corner
  // transform *= glm::mat3({
  //   // Move to origin
  //   1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   1.0, 1.0, 1.0
  // }) * glm::mat3({
  //   // Rotate
  //   std::cos(time), std::sin(time), 0.0,
  //   -std::sin(time), std::cos(time), 0.0,
  //   0.0, 0.0, 1.0
  // }) * glm::mat3({
  //   // Move back
  //   1.0, 0.0, 0.0,
  //   0.0, 1.0, 0.0,
  //   -1.0, -1.0, 1.0
  // });

  // Send matrix value to program
  auto transform_uniform = program->GetUniformLocation("Transform");
  glUniformMatrix3fv(transform_uniform, 1, GL_FALSE, glm::value_ptr(transform));
}

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    return EXIT_FAILURE;
  }

  // Setup OpenGL context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Try to create a window
  auto window = glfwCreateWindow( SIZE, SIZE, "PPGSO gl_gradient", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Finalize window setup
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_3")) {
    std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Load shaders
  auto program = ShaderPtr(new Shader{u5_vert, u5_frag});
  program->Use();

  // Generate a vertex array object
  // This keeps track of what attributes are associated with buffers
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Setup geometry
  std::vector<GLfloat> vertex_buffer{
          // x, y
          -0.5f,0.5f,
          -0.5f,0.3f,
          0.5f,0.5f,

          0.5f,0.3f,
          -0.5f,0.3f,
          0.5f,0.5f,

          -0.15f,0.30f,
          -0.15f,-0.5f,
          0.15f, -0.5f,

          0.15f, 0.30f,
          -0.15f,0.30f,
          0.15f, -0.5f,

  };

  // Generate a vertex buffer object, this will feed data to the vertex shader
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

  // Setup vertex array lookup, this tells the shader how to pick data for the "Position" input
  auto position_attrib = program->GetAttribLocation("Position");
  glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attrib);

  // Colors for vertices
  std::vector<GLfloat> color_buffer{
          // r, g, b
          1.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f,
          0.0, 0.0f, 1.0f,
          1.0f, 1.0f, 1.0f
  };

  GLuint cbo;
  glGenBuffers(1, &cbo);
  glBindBuffer(GL_ARRAY_BUFFER, cbo);
  glBufferData(GL_ARRAY_BUFFER, color_buffer.size() * sizeof(GLfloat), color_buffer.data(), GL_STATIC_DRAW);

  // Same thing for colors
  auto color_attrib = program->GetAttribLocation("Color");
  glVertexAttribPointer(color_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(color_attrib);

    float time=0;
  // Main execution loop
  while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 ) {
    // Set gray background
    glClearColor(.5,.5,.5,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      time+=0.01;
      SetTransformation(program,time);
    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLES, 0, 12);
//    glDrawElements(GL_POINTS,1,GL_FLOAT,&vertex_buffer);
    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glfwTerminate();

  return EXIT_SUCCESS;
}
