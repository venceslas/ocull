#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <drone_scene/drone_scene.hpp>

#include "engine/renderer/Program.hpp"
#include "engine/renderer/VertexBuffer.hpp"

class Camera;

namespace Ocull {
class Scene;
class Query;
class IndexList;
class Camera;
}


//dummy struct
struct Sampler
{
  unsigned int textureId;
};

struct Render
{
  std::vector<VertexBuffer> primitives;

  std::vector<bool> meshInitialized;    

  // unused xxx
  std::vector<int32_t> meshFrameUploaded; // index of the mesh frame uploaded  
  std::vector<uint64_t> meshIdx;//
  std::vector<uint32_t> samplerIdx;//

  // Textures
  std::vector<Sampler> samplers;
};



class Scene
{
  public:
    drn_t m_cache;    
    drn_scene::Scene m_drnScene;
    
    Render m_render;
    
    Program m_program;
    
    // Ocull datas
    Ocull::Scene *m_OcullScene;
    Ocull::Query *m_OcullQuery;
    Ocull::IndexList *m_OcullIndexList;
    Ocull::Camera *m_OcullCamera;
    
    
  public:
    Scene();
    ~Scene();
    
    void init(const char *filename);    
    void render(const Camera& camera);
    
  private:
    void initGeometry(const char *filename);
    void initShader();
    void initQuery();
    
    void updateGeometry();
};

#endif //SCENE_HPP_
