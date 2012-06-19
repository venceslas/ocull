///
///
///

#include "Scene.hpp"
#include <glsw/glsw.h>

#include "Data.hpp"
#include "engine/Camera.hpp"
#include "engine/VertexBuffer.hpp"


namespace app {

namespace cubewire {
void setup(engine::VertexBuffer &vb);
}

// -----------------------------------------------
// Constructor / Destructor
// -----------------------------------------------

Scene::Scene() 
    : m_bFileLoaded(false),
      m_bSceneResolved(false)
{
}

Scene::~Scene()
{
  if (m_bFileLoaded) {
    drn_close( &m_cache );
  }
  if (m_bSceneResolved) {    
    drn_scene::releaseScene(&m_drnScene);
  }
}


// -----------------------------------------------
// Initializers
// -----------------------------------------------

void Scene::init(const char *filename)
{
  initGeometry(filename);
  initShader();
  initQuery();  
}

void Scene::initGeometry(const char *filename)
{
  /// Open a DRONE file
  if (!filename || drn_open( &m_cache, filename, DRN_READ_MMAP) < 0)
  {
    fprintf( stderr, "\"%s\" is an invalid filename.\n", filename);
    exit( EXIT_FAILURE );
  }
  m_bFileLoaded = true;
  
  /// Check if it matches the 'DRONE scene' format
  if (0 != strcmp(drn_get_description(&m_cache), DRN_SCENE_CACHE_DESCRIPTION))
  {
    fprintf( stderr, "\"%s\" is an invalid drone scene file.\n", filename);
    exit( EXIT_FAILURE );
  }

  /// Retrieve the scene chunk id
  drn_chunk_id_t sceneEntryId;  
  {
    uint64_t status;
    
    status = drn_get_matching_chunks( 
                  &m_cache, 
                  drn_get_map_id( &m_cache, DRN_SCENE_CHUNK_TYPE_TAG), 
                  DRN_SCENE_CHUNK_TAG_TYPE_VALUE, 
                  1, 
                  &sceneEntryId);
    
    if (1u != status)
    {
      fprintf( stderr, "Error: could not retrieve the scene chunk id.\n", filename);
      exit( EXIT_FAILURE );
    }
  }

  /// Retrieve scene datas
  drn_scene::resolveScene(&m_cache, &m_drnScene, drn_get_chunk(&m_cache, sceneEntryId)); // valgrind errors XXX
  m_bSceneResolved = true;
  
  /// Resize global buffers
  m_meshInit.resize(m_drnScene.numMeshes, false);  
  m_primitives.resize(m_drnScene.numMeshes);
  
  //---------------
  
  cubewire::setup( m_cubeWire );
}

void Scene::initShader()
{
  const char *kShadersPath = "data/shader/";
  
  /// GLSW, shader file manager
  glswInit();
  
  glswSetPath( kShadersPath, ".glsl");
  glswAddDirectiveToken("*", "#version 330 core");  
  
  m_program.generate();
    m_program.addShader( engine::VERTEX_SHADER,   "PassThrough.Vertex");
    m_program.addShader( engine::FRAGMENT_SHADER, "PassThrough.Fragment");
  m_program.link();
  
  glswShutdown();
}

void Scene::initQuery()
{
}


// -----------------------------------------------
// Update
// -----------------------------------------------

void Scene::updateGeometry()
{
  /// actually there is an initial Init step & then a per frame update for
  /// animated mesh. Here just the basic mesh is loaded, so we could avoid
  /// the per frame update.
  
  for (uint64_t i = 0; i < m_primitives.size(); ++i)
  {    
    engine::VertexBuffer *p = &m_primitives[i];
    drn_scene::Mesh *m = &m_drnScene.meshes[i];
    
    if (0 == m->numTriangles) {
      continue;
    }
    
    /// first time init
    if (!m_meshInit[i])
    {    
      const int kFrameId = 0;
      
      // Vertices
      std::vector<glm::vec3> &positions = p->getPositions();  
      positions.resize( m->numHwVertices );
            
      const float *vertices = m->dynamicData[ kFrameId ].hwVertices;
      for (size_t k=0u; k<positions.size(); ++k)
      {
        positions[k] = glm::vec3( vertices[3u*k+0u],
                                  vertices[3u*k+1u],
                                  vertices[3u*k+2u] );
      }
      
      // Normals
      std::vector<glm::vec3> &normals = p->getNormals();
      normals.resize( m->numHwVertices );
            
      const float *nor = m->dynamicData[ kFrameId ].hwNormals;
      for (size_t k=0u; k<normals.size(); ++k)
      {
        normals[k] = glm::vec3( nor[3u*k+0u],
                                nor[3u*k+1u],
                                nor[3u*k+2u] );
      }
      
      // Indices
      std::vector<GLuint> &indices = p->getIndices();
      indices.assign( m->triangleList, &(m->triangleList[3*m->numTriangles]) );


      ///----
      
      /// setup Ocull Scene
      
      // TODO
      
      ///----
            
      p->generate();//
      p->complete( GL_STATIC_DRAW ); //
      p->cleanData();
      
      m_meshInit[i] = true;
    }
    
    /// Per frame update
    // (empty)
  }
}


// -----------------------------------------------
// Render
// -----------------------------------------------

void Scene::run(Data &data)
{
  // UPDATE  
  
  updateGeometry();
  
  ///-----------

  // RENDERING
  
  glClearColor( 0.2f, 0.4f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glDisable( GL_CULL_FACE );
  

  sf::Vector2u windowSize = data.context.handle.getSize();
  engine::Camera &mainCamera = data.view.camera[Data::View::CAMERA_MAIN];
  engine::Camera &debugCamera = data.view.camera[Data::View::CAMERA_DEBUG];

  // TODO better customisation
  if (data.ui.displayMode == Data::UI::DISPLAY_SPLITTED)
  {    
    //TODO change the projection matrix, or resize the window
    // (currently done in EventHandler..)
    
    int halfWidth = 0.5f * windowSize.x;
        
    glViewport( 0, 0, halfWidth, windowSize.y);
    render( mainCamera );
        
    glViewport( halfWidth, 0, windowSize.x-halfWidth, windowSize.y);
    render( debugCamera );    

    if (data.debug.bShowMainFrustum) {
      renderMainFrustum( mainCamera, debugCamera);
    }
  }
  else
  {
    glViewport( 0, 0, windowSize.x, windowSize.y);
    render( data.view.camera[data.view.active] );
    
    if (data.view.active == Data::View::CAMERA_DEBUG) {    
      if (data.debug.bShowMainFrustum) {
        renderMainFrustum( mainCamera, debugCamera);
      }
    }
    
  }
  
}

void Scene::render(const engine::Camera& camera)
{
  m_program.bind();  

  const glm::mat4 &viewProj = camera.getViewProjMatrix();
  
  for (uint64_t i = 0u; i < m_primitives.size(); ++i)  //
  {
    engine::VertexBuffer *p = &(m_primitives[i]);
    
    drn_scene::Mesh *m = &(m_drnScene.meshes[i]);

    uint64_t numDagNodes = m_drnScene.numDagNodes;
    uint64_t did = m->dagNodeId;

    uint32_t matid = m_drnScene.dagNodes[did].materialId;
    const drn_scene::Material *mat = m_drnScene.materials + matid;      
    
    
    // Matrix copy
    glm::mat4 worldMatrix;
    memcpy( &(worldMatrix[0][0]), m_drnScene.dagNodeStates[did].otwTransform, 
            sizeof(worldMatrix) );
    
    m_program.setUniform( "uModelViewProjMatrix", viewProj * worldMatrix);
    
    m_program.setUniform( "uColor", glm::vec3(mat->diffuseColor[0], 
                                              mat->diffuseColor[1], 
                                              mat->diffuseColor[2]));
    m_program.setUniform( "uEnableLight", true);
    
    p->draw();
  }

  m_program.unbind();
  
#if 0
  if (cameraType == CAMERA_DEBUG)
  {
    // Render the main frustum
    if (data.ui.bShowMainFrustum)
    {
      renderMainFrustum(camera);
    }
  }
#endif
}

void Scene::renderMainFrustum(const engine::Camera &mainCamera,
                              const engine::Camera &debugCamera)
{
  const glm::mat4 model = glm::inverse( mainCamera.getViewProjMatrix() );  
  const glm::mat4 viewProj = debugCamera.getViewProjMatrix();
  
  m_program.bind();  
  
    m_program.setUniform( "uModelViewProjMatrix", viewProj * model);    
    m_program.setUniform( "uColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_program.setUniform( "uEnableLight", false);
    
    m_cubeWire.draw(GL_LINES);
                                              
  m_program.unbind();
}

namespace cubewire {

void setup(engine::VertexBuffer& vb)
{  
  float vertices[] = 
  {
    -1.0f, -1.0f, -1.0f, //0
    -1.0f, -1.0f, +1.0f, //1
    -1.0f, +1.0f, -1.0f, //2
    -1.0f, +1.0f, +1.0f, //3
    +1.0f, -1.0f, -1.0f, //4
    +1.0f, -1.0f, +1.0f, //5
    +1.0f, +1.0f, -1.0f, //6
    +1.0f, +1.0f, +1.0f  //7
  };
  
  unsigned int lineIndices[] = 
  {
    0, 1,  0, 2,  0, 4,  1, 3,  1, 5,  2, 3,
    2, 6,  3, 7,  4, 5,  4, 6,  5, 7,  6, 7
  };
  
  size_t vertices_size = (sizeof(vertices) / sizeof(vertices[0]));
  size_t lineIndices_size = (sizeof(lineIndices) / sizeof(lineIndices[0]));
  
  std::vector<glm::vec3> &positions = vb.getPositions();
  positions.resize( vertices_size/3 );
  for (int i=0; i<positions.size(); ++i) {
    positions[i] = glm::vec3(vertices[3*i+0], vertices[3*i+1], vertices[3*i+2]);
  }
  
  std::vector<GLuint> &indices = vb.getIndices();
  indices.assign( lineIndices, lineIndices + lineIndices_size);
  
  vb.complete( GL_STATIC_DRAW );
  vb.cleanData();
}

} // namespace cubewire


} //namespace app
