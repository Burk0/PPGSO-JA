
#include <iostream>
#include <vector>
#include <map>
#include <list>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene.h"
#include "camera.h"
#include "player.h"
#include "space.h"
#include "block.h"
#include "mapka.h"

const unsigned int SIZE = 512;

Scene scene;

// Set up the scene
void InitializeScene() {
  scene.objects.clear();
  auto kuzel=KuzelPtr(new Kuzel{});
  kuzel->position.x=-0.5f;
  kuzel->position.y=1.2f;
  scene.objects.push_back(kuzel);
  // Create a camera
  auto camera = CameraPtr(new Camera{ 60.0f, 1.0f, 0.1f, 100.0f});
  camera->position.z = -15.0f;
  scene.camera = camera;

  // Add space background
  auto space = SpacePtr(new Space{});
  scene.objects.push_back(space);

    auto generator = MapkaPtr(new Mapka{scene});
    generator->position.y = 10.0f;
    scene.objects.push_back(generator);

  // Add player to the scene
  auto player = PlayerPtr(new Player{scene, kuzel});
  player->position.y = -7;
    player->position.x =+7;
  scene.objects.push_back(player);
}

// Keyboard press event handler
void OnKeyPress(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */) {
  scene.keyboard[key] = action;

  // Reset
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    InitializeScene();
  }
}

// Mouse move event handler
void OnMouseMove(GLFWwindow* /* window */, double xpos, double ypos) {
  scene.mouse.x = xpos;
  scene.mouse.y = ypos;
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
  auto window = glfwCreateWindow(2*SIZE, SIZE, "PPGSO gl_scene", nullptr, nullptr);
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

  // Add keyboard and mouse handlers
  glfwSetKeyCallback(window, OnKeyPress);
  glfwSetCursorPosCallback(window, OnMouseMove);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide mouse cursor
  glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

  // Initialize OpenGL state
  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable polygon culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  InitializeScene();

  // Track time
  float time = (float)glfwGetTime();

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Compute time delta
    float dt = (float)glfwGetTime() - time;
    time = (float)glfwGetTime();

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render all objects
    scene.Update(dt);
    scene.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
