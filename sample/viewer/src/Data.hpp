///
///   Data.hpp
///
///   Structure encapsulating the application global datas
///

#ifndef APP_DATA_HPP_
#define APP_DATA_HPP_

#include <SFML/Window.hpp>
#include "engine/Camera.hpp"


namespace app {

struct Data 
{
  struct Context {
    sf::Window handle;//
  } context;

  struct View 
  {
    enum CameraType 
    { 
      CAMERA_MAIN = 0, 
      CAMERA_DEBUG, 
      
      kCount 
    } active;
    
    engine::Camera camera[kCount];//
  } view;

  struct Debug 
  {
    bool bShowMainFrustum;
    bool bShowOccluders;
  } debug;

  struct Stats
  {
    unsigned int numTriSent;
    unsigned int numTriPassed;
    unsigned int numSamplesPassed;    
    unsigned int fps;
    float elapsedTime;
  } stats;

  struct UI
  {
    enum DisplayMode 
    { 
      DISPLAY_SIMPLE = 0, 
      DISPLAY_SPLITTED, 
      
      kCount 
    } displayMode;
    
    bool bShowStats;
    bool bShowHelp;
  } ui;


  // ------------------------------------


  Data()
  {
    view.active = View::CAMERA_MAIN;
    
    debug.bShowMainFrustum = true;
    debug.bShowOccluders = false;
    
    stats.numTriSent = 0u;
    stats.numTriPassed = 0u;
    stats.numSamplesPassed = 0u;
    stats.fps = 0u;
    stats.elapsedTime = 0.0f;
    
    ui.displayMode = UI::DISPLAY_SIMPLE;
    ui.bShowStats = true;
    ui.bShowHelp = false;
  }
};

} //namespace app

#endif //APP_DATA_HPP_
