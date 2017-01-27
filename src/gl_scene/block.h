#ifndef PPGSO_BLOCK_H
#define PPGSO_BLOCK_H

#include <memory>

#include <GL/glew.h>

#include "scene.h"
#include "object.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"


class Block : public Object {
public:
    Block();
  ~Block();

  // Implement object interface
  bool Update(Scene &scene, float dt) override;
  void Render(Scene &scene) override;
private:

  static MeshPtr mesh;
  static ShaderPtr shader;
  static TexturePtr texture;
};
typedef std::shared_ptr<Block> BlockPtr;

#endif //PPGSO_BLOCK_H
