#ifndef PPGSO_LAVABLOCK_H
#define PPGSO_LAVABLOCK_H

#include <memory>

#include <GL/glew.h>

#include "scene.h"
#include "object.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

class LavaBlock : public Object {
public:
    LavaBlock();
    ~LavaBlock();

    // Implement object interface
    bool Update(Scene &scene, float dt) override;
    void Render(Scene &scene) override;
private:
    // Generate explosion on position and scale, produce N smaller asteroid pieces
//  void Explode(Scene &scene, glm::vec3 explPosition, glm::vec3 explScale, int pieces);

    static MeshPtr mesh;
    static ShaderPtr shader;
    static TexturePtr texture;
};
typedef std::shared_ptr<LavaBlock> LavaBlockPtr;

#endif //PPGSO_LAVABLOCK_H
