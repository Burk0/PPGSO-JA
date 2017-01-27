
#include "object_frag.h"
#include "object_vert.h"

#include "camera.h"
#include "scene.h"
#include "object.h"
#include "player.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#define PI 3.14159265358979323846f

Camera::Camera(float fow, float ratio, float near, float far) {
  float fowInRad = (PI/180.0f) * fow;

  projectionMatrix = glm::perspective(fowInRad, ratio, near, far);
  offset = 0;
  offsetz=-17;
  up = glm::vec3(0,1,0);
  position = glm::vec3(0,0,0);
  back = glm::vec3(0,0,-1);

}

Camera::~Camera() {
}
int temp=-1;
bool Camera::Update(Scene &scene, float dt) {
  glm::vec3 poz = glm::vec3(0, 0, 0);

  for (auto obj : scene.objects) {

      auto player = std::dynamic_pointer_cast<Player>(obj);
      if (!player) continue;
//        std::cout<<player->position.x << "\n";
      if (player->position.x < -5.0 && player->position.x > -7.0) {
          if (offset < 12) {
//              temp = 0;
              offset += 10 * dt;
          }
      }
      if(player->position.x > -4.0 && player->position.x < 1.0)
      {
          if (offset > 0) {
//              temp = -1;
              offset -= 10 * dt;
          }
      }
  }
  if(scene.keyboard[GLFW_KEY_A]){
    if(offset > -10){
      offset -= 20 * dt;
    }
  } else if(scene.keyboard[GLFW_KEY_D]){
    if(offset < 12){
      offset += 20 * dt;
    }
  } else if(scene.keyboard[GLFW_KEY_E]){
    offset = 0;
    offsetz=0;
  } else if(scene.keyboard[GLFW_KEY_W])
  {
      if(offsetz>-20){
        offsetz+=-20*dt;
      }
  } else if(scene.keyboard[GLFW_KEY_S])
  {
    if(offsetz<20){
      offsetz+=+20*dt;
    }
  }

  glm::vec3 poz1 = glm::vec3(offset, 5, offsetz);

  viewMatrix = glm::lookAt(poz1, poz, up);

  GenerateModelMatrix();
}

void Camera::Render(Scene &scene) {}