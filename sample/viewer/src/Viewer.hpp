#ifndef APP_VIEWER_HPP_
#define APP_VIEWER_HPP_

#include "engine/Application.hpp"

#include "Data.hpp"
#include "EventHandler.hpp"
#include "Scene.hpp"
#include "UserInterface.hpp"


namespace app {

class Viewer : public engine::Application
{    
  private:
    Data m_data;
    
    EventHandler m_eventHandler;
    Scene m_scene;
    UserInterface m_userInterface;
    
    
  public:
    void run();
  
  private:
    void _initContext( int argc, char *argv[]);
    void _initData( int argc, char *argv[]);
};

}

#endif //APP_VIEWER_HPP_
