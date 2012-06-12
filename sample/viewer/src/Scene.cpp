#include "Scene.hpp"

#include <glsw/glsw.h>
#include <ocull/ocull.hpp>//

#include "App.hpp"
#include "engine/Camera.hpp"


// -----------------------------------------------
// Constructor / Destructor
// -----------------------------------------------

Scene::Scene()
    : m_OcullScene(NULL)
{
}

Scene::~Scene()
{
  // (close it before ?)
  drn_close( &m_cache );
  
  if (m_OcullScene) {
    delete m_OcullScene;
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
  
  /// Check if it matches the 'DRONE scene' format
  if (0 != strcmp(drn_get_description(&m_cache), DRN_SCENE_CACHE_DESCRIPTION))
  {
    fprintf( stderr, "\"%s\" is an invalid drone scene file.\n", filename);
    drn_close( &m_cache );
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
  
  
  drn_scene::Scene &scene = m_drnScene;
  
  /// Retrieve scene datas
  drn_scene::resolveScene(&m_cache, &scene, drn_get_chunk(&m_cache, sceneEntryId));
  
  /// Resize global buffers
  m_render.meshInitialized.resize( scene.numMeshes, false);  
  m_render.meshIdx.resize(scene.numMeshes);
  m_render.samplerIdx.resize(scene.numMeshes);
  m_render.primitives.resize(scene.numMeshes);
  m_render.samplers.resize(scene.numMaterials);
  
  
  /// Setup scene meshes data
  for (size_t i=0u; i<scene.numMeshes; ++i)
  { 
    // non dynamic mesh could be set here
    
    m_render.meshIdx[i] = i;
    
    size_t dagNodeId = scene.meshes[i].dagNodeId;
    m_render.samplerIdx[i] = scene.dagNodes[dagNodeId].materialId;
  }


  /// Setup scene materials data
  for (size_t i=0u; i<scene.numMaterials; ++i)
  {
    drn_scene::Texture *pDiffuseTex = scene.materials[i].diffuseTexture;
    
    if (NULL == pDiffuseTex) 
    {
      m_render.samplers[i].textureId = 0u;
      continue;
    }

    
    GLenum channels = (3 == pDiffuseTex->ncomp) ? GL_RGB :
                      (1 == pDiffuseTex->ncomp) ? GL_LUMINANCE :
                                                  GL_RGBA;
    
    GLuint texId = m_render.samplers[i].textureId;

    /// [TODO] create texture (tmp version) :
    glGenTextures( 1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, channels, pDiffuseTex->width, pDiffuseTex->height, 0, channels, 
                                GL_UNSIGNED_BYTE, pDiffuseTex->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0u);
  }
  
}

void Scene::initShader()
{
  /// GLSW, shader file manager
  glswInit();
  glswSetPath( App::kShadersPath, ".glsl");
  glswAddDirectiveToken("*", "#version 330 core");  
  
  m_program.generate();
    m_program.addShader( VERTEX_SHADER, "PassThrough.Vertex");
    m_program.addShader( FRAGMENT_SHADER, "PassThrough.Fragment");
  m_program.link();
  
  glswShutdown();
}

void Scene::initQuery()
{
  /*
  /// Specify the camera + frustum for the current frame
  Ocull::Frustum frustum( fov, aspectRatio, zNear, zFar);
  Ocull::Matrix4x4 viewMatrix = setViewMatrix(); //
  
  m_OcullCamera = new Ocull::Camera( frustum, viewMatrix);
  */
  
  //m_OcullScene = Ocull::Scene::Create();
  m_OcullScene = new Ocull::Scene();
}

// -----------------------------------------------
// Update
// -----------------------------------------------


void Scene::updateGeometry()
{
  /// actually there is an initial Init step & then a per frame update for
  /// animated mesh. Here just the basic mesh is loaded, so we could avoid
  /// the per frame update.
  
  for (uint64_t i = 0; i < m_render.primitives.size(); ++i)
  {    
    VertexBuffer *p = &m_render.primitives[i];
    drn_scene::Mesh *m = &m_drnScene.meshes[m_render.meshIdx[i]];
    
    if (0 == m->numTriangles) {
      continue;
    }
    
    /// first time init
    if (!m_render.meshInitialized[i])
    {    
      const int frameId = 0;
      
      // Vertices
      std::vector<glm::vec3> &positions = p->getPositions();  
      positions.resize( m->numHwVertices );
            
      const float *vertices = m->dynamicData[frameId].hwVertices;
      for (size_t k=0u; k<positions.size(); ++k)
      {
        positions[k] = glm::vec3( vertices[3u*k+0u],
                                  vertices[3u*k+1u],
                                  vertices[3u*k+2u] );
      }
      
      // Normals
      std::vector<glm::vec3> &normals = p->getNormals();
      normals.resize( m->numHwVertices );
            
      const float *nor = m->dynamicData[frameId].hwNormals;
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
      
      const Ocull::Model *pModel = m_OcullScene->insertModel(vertices, 
                                                             &indices[0], 
                                                             m->numHwVertices, 
                                                             m->numTriangles);
      
      uint64_t did = m->dagNodeId;
      
      glm::mat4 worldMatrix;
      const float *pMat = m_drnScene.dagNodeStates[did].otwTransform;
      memcpy( &(worldMatrix[0][0]), pMat, 16*sizeof(float));//
            
      m_OcullScene->insertObject( pModel, worldMatrix, i);
      
      ///----
      
      
      p->complete( GL_STATIC_DRAW ); //
      p->cleanData();
      
      m_render.meshInitialized[i] = true;
    }
    
    /// Per frame update
    // (empty)
  }
  
}


// -----------------------------------------------
// Render
// -----------------------------------------------

void Scene::render(const Camera& camera)
{ 
  // UPDATE  
  updateGeometry();
  
  //-----------
  
  // RENDERING
  
  glClearColor( 0.2f, 0.4f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glDisable( GL_CULL_FACE );
  
  
  m_program.bind();
  
  const glm::mat4 &viewProj = camera.getViewProjMatrix();
    
  for (uint64_t i = 0; i < m_render.primitives.size(); ++i)  //
  {
    VertexBuffer *p = &(m_render.primitives[i]);
    drn_scene::Mesh *m = &(m_drnScene.meshes[ m_render.meshIdx[i] ]);

    uint64_t numDagNodes = m_drnScene.numDagNodes;
    uint64_t did = m->dagNodeId;
    uint32_t matid = m_drnScene.dagNodes[did].materialId;
    
    const drn_scene::Material *mat = m_drnScene.materials + matid;
    
    /*
    uint32_t frame = drn_scene::timeToFrame(states->time,
                                            s->timeRange.startFrame,
                                            s->timeRange.endFrame,
                                            s->timeRange.subFrames);
    */
    
    GLuint texId = m_render.samplers[m_render.samplerIdx[i]].textureId;
    glBindTexture( GL_TEXTURE_2D, texId);
    
    
    glm::mat4 worldMatrix;
    
#if 1
    memcpy( &(worldMatrix[0][0]), m_drnScene.dagNodeStates[did].otwTransform, 16*sizeof(float) );
#else
    for (int i=0;i<4;++i)
    {
      for (int j=0;j<4;++j)
      {
        worldMatrix[i][j] = m_drnScene.dagNodeStates[did].otwTransform[4*i+j];
      }
    }
#endif
    
    m_program.setUniform( "uModelViewProjMatrix", viewProj * worldMatrix);
    
    m_program.setUniform( "uColor", glm::vec3(mat->diffuseColor[0], 
                                              mat->diffuseColor[1], 
                                              mat->diffuseColor[2])); //
    
    assert( glGetError() == 0 );//
    p->draw();
    assert( glGetError() == 0 );//
  }
  glBindTexture( GL_TEXTURE_2D, 0u);

  m_program.unbind();  
}
