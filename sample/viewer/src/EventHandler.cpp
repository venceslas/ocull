///
///
///

#include "EventHandler.hpp"
#include "Data.hpp"


namespace app {

void EventHandler::handle(Data &data)
{
  sf::Window &window = data.context.handle;

  m_bExit = false;
  
  while (window.pollEvent(m_event))
  {
    switch (m_event.type)
    {
      case sf::Event::Closed:
        eventClosed(data); 
      break;
      
      case sf::Event::Resized:
        eventResized(data);
      break;
      
      case sf::Event::LostFocus:
        eventLostFocus(data); 
      break;
      
      case sf::Event::GainedFocus:
        eventGainedFocus(data); 
      break;
      
      case sf::Event::TextEntered:
        eventTextEntered(data); 
      break;
      
      case sf::Event::KeyPressed:
        eventKeyPressed(data); 
      break;
      
      case sf::Event::KeyReleased:	
        eventKeyReleased(data); 
      break;
      
      case sf::Event::MouseWheelMoved:	    
        eventMouseWheelMoved(data);
      break;
      
      case sf::Event::MouseButtonPressed:	  
        eventMouseButtonPressed(data); 
      break;
      
      case sf::Event::MouseButtonReleased:	
        eventMouseButtonReleased(data); 
      break;
      
      case sf::Event::MouseMoved:
        eventMouseMoved(data);
      break;
      
      case sf::Event::MouseEntered:
        eventMouseEntered(data);
      break;
      
      case sf::Event::MouseLeft:
        eventMouseLeft(data);
      break;
      
      case sf::Event::JoystickButtonPressed:
        eventJoystickButtonPressed(data);
      break;
      
      case sf::Event::JoystickButtonReleased:
        eventJoystickButtonReleased(data);
      break;
      
      case sf::Event::JoystickMoved:
        eventJoystickMoved(data);
      break;
      
      case sf::Event::JoystickConnected:
        eventJoystickConnected(data);
      break;
      
      case sf::Event::JoystickDisconnected:
        eventJoystickDisconnected(data);
      break;
      
      default:
      break;
    }
  }
  
  if (m_bExit)
  {
	  sf::Window &window = data.context.handle;
    window.close();
  }
}

// --------------------------------------------------

void EventHandler::eventClosed(Data &data)
{
  m_bExit = true;
}

void EventHandler::eventResized(Data &data)
{
}

void EventHandler::eventLostFocus(Data &data)
{
}

void EventHandler::eventGainedFocus(Data &data)
{
}

void EventHandler::eventTextEntered(Data &data)
{
}

void EventHandler::eventKeyPressed(Data &data)
{
  sf::Window &window = data.context.handle;
  engine::Camera &camera = data.view.camera[data.view.active];
  
  switch (m_event.key.code)
  {
    case sf::Keyboard::Escape:
      m_bExit = true;
    break;
    
    case sf::Keyboard::H:
      data.ui.bShowHelp = true;
    break;
    
    case sf::Keyboard::F1:
    {
      // toggle through the display modes
      int mode = (data.ui.displayMode+1) % Data::UI::kCount;
      data.ui.displayMode = Data::UI::DisplayMode(mode);
      
      // XXX
      sf::Vector2u windowSize = window.getSize();
      float ratio = windowSize.x / float(windowSize.y);
      if (mode == 1) {
        data.view.camera[Data::View::CAMERA_MAIN].updateProjectionAspectRatio( 0.5f*ratio );
        data.view.camera[Data::View::CAMERA_DEBUG].updateProjectionAspectRatio( 0.5f*ratio );
      } else {
        data.view.camera[Data::View::CAMERA_MAIN].updateProjectionAspectRatio( ratio );
        data.view.camera[Data::View::CAMERA_DEBUG].updateProjectionAspectRatio( ratio );
      }
    }
    break;
    
    case sf::Keyboard::F2:
      data.ui.bShowStats = !data.ui.bShowStats;
    break;
    
    /// CAMERA MOVEMENT
    case sf::Keyboard::Z:
      camera.keyboardHandler( engine::MOVE_FORWARD, true);
    break;
    
    case sf::Keyboard::S:
      camera.keyboardHandler( engine::MOVE_BACKWARD, true);
    break;
    
    case sf::Keyboard::Q:
      camera.keyboardHandler( engine::MOVE_LEFT, true);
    break;
    
    case sf::Keyboard::D:
      camera.keyboardHandler( engine::MOVE_RIGHT, true);
    break;
    
        
    default:
    break;
  }
}

void EventHandler::eventKeyReleased(Data &data)
{
  engine::Camera &camera = data.view.camera[data.view.active];
 
  
  switch (m_event.key.code)
  {
    case sf::Keyboard::H:
      data.ui.bShowHelp = false;
    break;
    
    case sf::Keyboard::Z:
      camera.keyboardHandler( engine::MOVE_FORWARD, false);
    break;
    
    case sf::Keyboard::S:
      camera.keyboardHandler( engine::MOVE_BACKWARD, false);
    break;
    
    case sf::Keyboard::Q:
      camera.keyboardHandler( engine::MOVE_LEFT, false);
    break;
    
    case sf::Keyboard::D:
      camera.keyboardHandler( engine::MOVE_RIGHT, false);
    break;
    
    default:
    break;
  }
}

void EventHandler::eventMouseWheelMoved(Data &data)
{
}

void EventHandler::eventMouseButtonPressed(Data &data)
{
  sf::Window &window = data.context.handle;
  
  if (m_event.mouseButton.button == sf::Mouse::Left)
  {
    // In splitted mode, change the active camera
    if (data.ui.displayMode == Data::UI::DISPLAY_SPLITTED)
    {    
      sf::Vector2u windowSize = window.getSize();    
      sf::Vector2i mouseRelPos = sf::Mouse::getPosition(window);
      
      int halfWidth = 0.5f*windowSize.x;
      data.view.active = (mouseRelPos.x < halfWidth) ? Data::View::CAMERA_MAIN :
                                                       Data::View::CAMERA_DEBUG;
    }
    
    // update the active camera position
    engine::Camera &camera = data.view.camera[data.view.active];
    camera.motionHandler( m_event.mouseButton.x, m_event.mouseButton.y, true);    
  }
}

void EventHandler::eventMouseButtonReleased(Data &data)
{
}

void EventHandler::eventMouseMoved(Data &data)
{
  engine::Camera &camera = data.view.camera[data.view.active];
  
  bool bLeftClick = sf::Mouse::isButtonPressed( sf::Mouse::Left );
  
  if (bLeftClick) {
    camera.motionHandler( m_event.mouseMove.x, m_event.mouseMove.y, false); 
  }
}

void EventHandler::eventMouseEntered(Data &data)
{
}

void EventHandler::eventMouseLeft(Data &data)
{
}

void EventHandler::eventJoystickButtonPressed(Data &data)
{
}

void EventHandler::eventJoystickButtonReleased(Data &data)
{
}

void EventHandler::eventJoystickMoved(Data &data)
{
}

void EventHandler::eventJoystickConnected(Data &data)
{
}

void EventHandler::eventJoystickDisconnected(Data &data)
{
}

} //namespace app
