#include "player.h"
//#include "scene.h"
#include "block.h"
#include "lavaBlock.h"
#include "kuzel.h"
#include "finishBlock.h"

#include "object_frag.h"
#include "object_vert.h"

#include <GLFW/glfw3.h>

float gravity = 9.8f;
float velocityY=7.0f;
int colizia=0;
//auto kuzel=KuzelPtr(new Kuzel());
Player::Player(Scene &scene, KuzelPtr kuzel) {

  scale *= 0.5f;
    jumpDelay = 0;
    // Set the rate of fire
    jump=false;
    kuzelptr=kuzel;

  // Initialize static resources if needed
  if (!shader) shader = ShaderPtr(new Shader{object_vert, object_frag});
  if (!texture) texture = TexturePtr(new Texture{"silver.rgb", 512/2, 512/2});
  if (!mesh) mesh = MeshPtr(new Mesh{shader, "sphere.obj"});
}

Player::~Player() {
}
int tmp=-1;
bool Player::Update(Scene &scene, float dt) {
    if(!Collision(scene))
        return false;


    jumpDelay += dt;
    // Hit detection
    for (auto obj : scene.objects) {
        // Ignore self in scene
        if (obj.get() == this)
            continue;

        // Keyboard controls
        if (scene.keyboard[GLFW_KEY_LEFT]) {
            if(position.x<7)
            position.x += 0.08 * dt;

    } else if (scene.keyboard[GLFW_KEY_RIGHT]) {
            Collision2(scene);
            if(position.x>-21)
            {position.x -= 0.08 * dt;}
    } else {
        rotation.z = 0;
    }
}
    if (scene.keyboard[GLFW_KEY_SPACE]) {
//        if (!jump) {
            if(tmp==-1) {
                tmp = 0;
            }
//        }
  }
    if(tmp==0){
        velocityY -= (gravity * dt);
//        printf("%f\n", velocityY);
        position.y += velocityY * dt;
        if(position.y<=-7){
            velocityY=7.0f;
            tmp =-1;
        }
    }
    //velocityY += gravity * dt;
    kuzelptr->Update(scene,dt,modelMatrix);
  GenerateModelMatrix();
  return true;
}

void Player::Render(Scene &scene) {
  shader->Use();

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}

bool Player::Collision(Scene &scene) {

    for (auto obj : scene.objects) {

        auto lavablock = std::dynamic_pointer_cast<LavaBlock>(obj);
//        auto block = std::dynamic_pointer_cast<Block>(obj);
        if (!lavablock) continue;

        if (glm::distance(this->position, lavablock->position) < lavablock->scale.y + 0.5) {
//            scene.objects.clear();
            std::cout << "Game Over\n";
            return false;
        }
        return true;
    }
}

bool Player::Collision2(Scene &scene) {

    for (auto obj : scene.objects) {

        auto finishblock = std::dynamic_pointer_cast<finishBlock>(obj);
        if (!finishblock) continue;

        if (glm::distance(this->position, finishblock->position) < finishblock->scale.y) {
            std::cout << "Win\n";
            return false;
        }
    }
    return true;
}

// shared resources
MeshPtr Player::mesh;
ShaderPtr Player::shader;
TexturePtr Player::texture;