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
      m_bSceneResolved(false),
      m_screenmappingVAO(0u),
      m_ocullQuery(NULL)
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
  
  if (m_screenmappingVAO) {
    glDeleteVertexArrays( 1, &m_screenmappingVAO);
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
  
  //--------------
  
  glGenVertexArrays( 1, &m_screenmappingVAO); 
}

void Scene::initShader()
{
  const char *kShadersPath = "data/shader/";
  

  /// GLSW, shader file manager
  glswInit();
  
  glswSetPath( kShadersPath, ".glsl");
  glswAddDirectiveToken("*", "#version 330 core");  
  
  m_passthroughPS.generate();
    m_passthroughPS.addShader( engine::VERTEX_SHADER,   "PassThrough.Vertex");
    m_passthroughPS.addShader( engine::FRAGMENT_SHADER, "PassThrough.Fragment");
  m_passthroughPS.link();
  
  m_screenmappingPS.generate();
    m_screenmappingPS.addShader( engine::VERTEX_SHADER,   "ScreenMapping.Vertex");
    m_screenmappingPS.addShader( engine::FRAGMENT_SHADER, "ScreenMapping.Fragment");
  m_screenmappingPS.link();
  
  glswShutdown();
  
}

void Scene::initQuery()
{
  m_ocullContext = ocull::Context::Create( "data/shader/pipeShader.cubin" );
  
  
  // TODO set camera frustum..

  // mesh set in updateGeometry
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

            
      p->generate();//
      p->complete( GL_STATIC_DRAW ); //


      ///----
      /// setup Ocull Scene
#     if 0
      // OpenGL
      m_ocullMesh.set( p->getVBO(), 0u, p->getNumVertices(), 0u,
                       p->getIBO(), 0u, p->getNumIndices());
#     else
      // Cuda array
      m_ocullMesh.set( vertices, positions.size(),
                       &indices[0], indices.size());
#     endif
      ///----

      
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
  


  sf::Vector2u windowSize = data.context.handle.getSize();

# if 1

  ///-----------
  
  if (m_ocullQuery == NULL) {
    m_ocullQuery = new ocull::Query( windowSize.x, windowSize.y);//
  }
  
  
  //query XXX
  const engine::Camera &camera = data.view.camera[data.view.active];
    
  float ratio = windowSize.x / float(windowSize.y);
  m_ocullCamera.setFrustum( ocull::Frustum(60.0f, ratio, 0.5, 2500.0f) );
  
  m_ocullCamera.setViewMatrix( camera.getViewMatrix() );
  m_ocullQuery->setCamera( m_ocullCamera );
  
  ocull::Matrix4x4 identity;
  
  m_ocullContext->begin( m_ocullQuery );
    m_ocullContext->uploadMesh( &m_ocullMesh, identity);
  m_ocullContext->end();
  
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  screenMapping( m_ocullContext->getColorTexture() );
    
  /**/
  ///-----------
  
# else

  // RENDERING
  glClearColor( 0.2f, 0.2f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glDisable( GL_CULL_FACE );
  

  engine::Camera &mainCamera = data.view.camera[Data::View::CAMERA_MAIN];
  engine::Camera &debugCamera = data.view.camera[Data::View::CAMERA_DEBUG];

  // TODO better customization
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

# endif
  
}

void Scene::render(const engine::Camera& camera)
{
  m_passthroughPS.bind();  

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
    
    m_passthroughPS.setUniform( "uModelViewProjMatrix", viewProj * worldMatrix);
    
    m_passthroughPS.setUniform( "uColor", glm::vec3(mat->diffuseColor[0], 
                                                    mat->diffuseColor[1], 
                                                    mat->diffuseColor[2]));
    m_passthroughPS.setUniform( "uEnableLight", true);
    
    // bug with large scene: too much VAOs created ?
    p->draw();
    
    //printf("%d\n", glGetError());
  }

  m_passthroughPS.unbind();
}


void Scene::renderMainFrustum(const engine::Camera &mainCamera,
                              const engine::Camera &debugCamera)
{
  const glm::mat4 model = glm::inverse( mainCamera.getViewProjMatrix() );  
  const glm::mat4 viewProj = debugCamera.getViewProjMatrix();
  
  m_passthroughPS.bind();  
  
    m_passthroughPS.setUniform( "uModelViewProjMatrix", viewProj * model);    
    m_passthroughPS.setUniform( "uColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_passthroughPS.setUniform( "uEnableLight", false);
    
    m_cubeWire.draw(GL_LINES);
                                              
  m_passthroughPS.unbind();
}

void Scene::screenMapping(const unsigned int textureId)
{
  glClear( GL_COLOR_BUFFER_BIT );
  
  glDisable( GL_DEPTH_TEST );
  glDepthMask( GL_FALSE );
  
  m_screenmappingPS.bind();
  {
    m_screenmappingPS.setUniform( "uTexture", 0);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textureId);

    glBindVertexArray( m_screenmappingVAO );
      glDrawArrays( GL_TRIANGLES, 0, 3);
    glBindVertexArray( 0u );

    glBindTexture( GL_TEXTURE_2D, 0u);
  }
  m_screenmappingPS.unbind();
  
  glDepthMask( GL_TRUE );
  glEnable( GL_DEPTH_TEST );
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

