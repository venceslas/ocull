#ifndef APP_HPP
#define APP_HPP

// Application layout
#include "engine/Application.hpp"

#include "Scene.hpp"
#include "engine/Camera.hpp"


/// =========================================
/// Sample application to test the cudaraster
/// port.
/// =========================================
class App : public Application
{ 
  public:
    // ++ Default window's resolution ++
    // (must be a multiple of CR_TILE_SIZE)
    static const int kScreenWidth  = 1080;
    static const int kScreenHeight = 720;  
    
    // ++ Default shaders' path ++
    static const char* kShadersPath;
    
    // ++ Holds rendering states ++
    static struct RenderState
    {
      bool bTexturing;
      bool bDepth;
      bool bBlend;
      bool bLerp;
      bool bQuads;
      int numSamples;
      
      RenderState()
        : bTexturing(false),
          bDepth(true),
          bBlend(false),
          bLerp(false),
          bQuads(false),
          numSamples(1)
      {}
    } kState;
  
  
  private:
    Scene m_scene;        
    Camera m_camera;
    
    
  public:
    App() 
      : Application()
    {}
    
    virtual ~App();

    // ++ Overrided Listener methods ++
    virtual void reshape(int w, int h);
    virtual void display();
    virtual void keyboard( unsigned char key, int x, int y);
    virtual void special( int key, int x, int y);
    virtual void specialUp( int key, int x, int y);
    virtual void mouse(int button, int state, int x, int y);
    virtual void motion(int x, int y);
    virtual void idle();
    
    
  private:
    // ++ Redefines the context (window + event) creation ++
    virtual void _initContext(int argc, char *argv[]);

    // ++ Redefines the generic datas initialization ++
    virtual void _initObject( int argc, char *argv[]);
};


#endif //APP_HPP
