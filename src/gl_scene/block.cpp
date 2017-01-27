#include "block.h"
//#include "explosion.h"

#include "object_frag.h"
#include "object_vert.h"

Block::Block() {

  // Initialize static resources if needed
  if (!shader) shader = ShaderPtr(new Shader{object_vert, object_frag});
  if (!texture) texture = TexturePtr(new Texture{"sphere.rgb", 64, 512/2});
  if (!mesh) mesh = MeshPtr(new Mesh{shader, "cube.obj"});
}

Block::~Block() {
}

bool Block::Update(Scene &scene, float dt) {
  GenerateModelMatrix();
  return true;
}


void Block::Render(Scene &scene) {
  shader->Use();

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}

// shared resources
MeshPtr Block::mesh;
ShaderPtr Block::shader;
TexturePtr Block::texture;
