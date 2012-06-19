#ifndef APP_EVENTHANDLER_HPP__
#define APP_EVENTHANDLER_HPP__

#include <SFML/Window.hpp>


namespace app {

struct Data;

class EventHandler
{
  private:
    sf::Event m_event;
    bool m_bExit;
    
  
  public:
    virtual void handle(Data &data);
    
  private:
    virtual void eventClosed(Data &data);
    virtual void eventResized(Data &data);
    virtual void eventLostFocus(Data &data);
    virtual void eventGainedFocus(Data &data);
    virtual void eventTextEntered(Data &data);
    virtual void eventKeyPressed(Data &data);
    virtual void eventKeyReleased(Data &data);
    virtual void eventMouseWheelMoved(Data &data);
    virtual void eventMouseButtonPressed(Data &data);
    virtual void eventMouseButtonReleased(Data &data);
    virtual void eventMouseMoved(Data &data);
    virtual void eventMouseEntered(Data &data);
    virtual void eventMouseLeft(Data &data);
    virtual void eventJoystickButtonPressed(Data &data);
    virtual void eventJoystickButtonReleased(Data &data);
    virtual void eventJoystickMoved(Data &data);
    virtual void eventJoystickConnected(Data &data);
    virtual void eventJoystickDisconnected(Data &data);
};

}

#endif //APP_EVENTHANDLER_HPP__


