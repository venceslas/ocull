#ifndef APP_SCENE_HPP__
#define APP_SCENE_HPP__

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <drone_scene/drone_scene.hpp>

#include "engine/VertexBuffer.hpp"
#include "engine/Program.hpp"



namespace engine {
class Camera;
}

namespace app {

struct Data;

class Scene
{
  public:
    bool m_bFileLoaded;
    bool m_bSceneResolved;
    
    // DRONE
    drn_t m_cache;    
    drn_scene::Scene m_drnScene;
    
    // Meshes
    std::vector<engine::VertexBuffer> m_primitives;
    std::vector<bool> m_meshInit;
    
    engine::Program m_program;
    
        
  public:
    Scene();
    ~Scene();
    
    void init(const char *filename);    
    void run(Data &data);
    
  private:
    void initGeometry(const char *filename);
    void initShader();
    void initQuery();
    
    void updateGeometry();
    
    void render(const engine::Camera& camera);
};

}

#endif //APP_SCENE_HPP__
