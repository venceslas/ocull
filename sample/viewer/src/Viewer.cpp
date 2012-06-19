///
///   Viewer.cpp
///

#include "Viewer.hpp"

#include <SFML/Window.hpp>
#include "engine/Camera.hpp"


namespace app {

void Viewer::run()
{
  sf::Window &window = m_data.context.handle;

  while (true)
  {
    m_eventHandler.handle( m_data );

    if (!window.isOpen()) {
      break;
    }

    // Update the camera !!
    m_data.view.camera[m_data.view.active].update();
    
    m_scene.run( m_data );
    
    // User interface (mostly text)
    m_userInterface.render( m_data );


    window.display();
  }
}

void Viewer::_initContext( int argc, char *argv[])
{
  sf::Window &window = m_data.context.handle;
  
  sf::ContextSettings settings;    
  settings.depthBits = 24u;
  settings.stencilBits = 0u;
  settings.antialiasingLevel = 0u;
  settings.majorVersion = 3u;
  settings.minorVersion = 3u;  
    
  window.create(sf::VideoMode( 1280, 720), 
                "Ocull viewer", 
                sf::Style::Default,
                settings);

  window.setVerticalSyncEnabled(false);
  window.setFramerateLimit(60);
}


void Viewer::_initData( int argc, char *argv[])
{
  m_scene.init( (argc>1) ? argv[1] : NULL );
  
  m_userInterface.init();
  
  //---------------
  
  
  // TODO rewrite
  
  sf::Vector2u windowSize = m_data.context.handle.getSize();
  
  const float fov = 60.0f;
  const float aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
  const float zNear = 0.5f;
  const float zFar = 2500.0f;
    
  
  // Main Camera
  engine::Camera *pCamera = &m_data.view.camera[Data::View::CAMERA_MAIN];
  pCamera->setViewParams( glm::vec3( 0.0f, 5.0f, 5.0f), 
                          glm::vec3( 0.0f, 5.0f, 0.0f) );
  pCamera->setProjectionParams( fov, aspectRatio, zNear, zFar);
  
  
  // Debug Camera
  pCamera = &m_data.view.camera[Data::View::CAMERA_DEBUG];
  pCamera->setViewParams( glm::vec3( 0.0f, 5.0f, 5.0f), 
                          glm::vec3( 0.0f, 5.0f, 0.0f) );
  pCamera->setProjectionParams( fov, aspectRatio, zNear, zFar);
}

}
