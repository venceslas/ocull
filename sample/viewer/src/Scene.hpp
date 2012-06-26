#ifndef APP_SCENE_HPP__
#define APP_SCENE_HPP__

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <drone_scene/drone_scene.hpp>

#include "engine/VertexBuffer.hpp"
#include "engine/Program.hpp"

#include <ocull/ocull.hpp> //


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
    
    // cube mesh to display the main frustum
    engine::VertexBuffer m_cubeWire;
    
    engine::Program m_program;
    
    ///    
    ocull::Context m_ocullContext;
    ocull::Query *m_ocullQuery;
    ocull::Camera m_ocullCamera;
    ocull::Mesh *m_ocullMesh;
    ///
    
  
  
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
    
    void renderMainFrustum(const engine::Camera &mainCamera,
                           const engine::Camera &debugCamera);
};

}

#endif //APP_SCENE_HPP__
