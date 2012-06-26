#include "UserInterface.hpp"

#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <gltext/gltext.hpp>


namespace {

void print_help()
{
  fprintf(stdout, 
          "[F1] toggle simple/splitted mode\n"\
          "[F2] show/hide statistics\n"\
          "[Z][Q][S][D] move camera\n"\
          "[mouse click + motion] rotate camera\n"
          //"[H] hold to display help"          
         );
}

}


namespace app {

void UserInterface::init()
{
  m_font = gltext::Font("./data/font/arialbd.ttf", 16, 128);
  m_bInitialized = true;
  
  print_help();
}

void UserInterface::render(const Data &data)
{
  assert( m_bInitialized );
  
  
  sf::Vector2u windowSize = data.context.handle.getSize();
    
  // (TODO put in a 'reshape' callback ?)
  m_font.setDisplaySize( windowSize.x, windowSize.y); //  
  glViewport( 0, 0, windowSize.x, windowSize.y); //  
  
  // Enables transparency
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  ///--------------------------------------------  
  
  /// Stats
  if (data.ui.bShowStats) {
    displayStats( data.stats, windowSize.x, windowSize.y);
  }
  
  /// Display mode  
  displayView( data.ui.displayMode, data.view, windowSize.x, windowSize.y);
  
  ///--------------------------------------------  
  
  glDisable( GL_BLEND );
}



void UserInterface::displayStats(const Data::Stats &stats, 
                                 unsigned int w, unsigned int h)
{  
  const size_t bufferSize = 128u;
  char sbuffer[bufferSize];
  
  
  // pen attributes
  const unsigned int ptSize = 14u;
  const unsigned int vspace = 3u;
  const unsigned int voffset = ptSize + vspace;
  
  m_font.setPointSize(ptSize);  
  m_font.setPenColor(1.0f, 1.0f, 0.8f);
  m_font.setPenAlpha(0.75f);  
  
  // pen position
  const unsigned int xPos = 16u;
        unsigned int yPos = h - voffset;    
  
  
  snprintf(sbuffer, bufferSize, "triangles sent: %u",  stats.numTriSent);
  m_font.setPenPosition( xPos, yPos);
  m_font.draw( sbuffer );
  yPos -= voffset;
  
  snprintf(sbuffer, bufferSize, "triangles passed: %u", stats.numTriPassed);
  m_font.setPenPosition( xPos, yPos);
  m_font.draw( sbuffer );
  yPos -= voffset;
  
  snprintf(sbuffer, bufferSize, "pixels passed: %u", stats.numSamplesPassed);
  m_font.setPenPosition( xPos, yPos);
  m_font.draw( sbuffer );
  yPos -= voffset;
  
  snprintf(sbuffer, bufferSize, "query time: %.2f ms", stats.elapsedTime);
  m_font.setPenPosition( xPos, yPos);
  m_font.draw( sbuffer );
  yPos -= voffset;
  
  snprintf(sbuffer, bufferSize, "%u FPS", stats.fps);
  m_font.setPenPosition( xPos, yPos);
  m_font.draw( sbuffer );
  yPos -= voffset;
}


void UserInterface::displayView(Data::UI::DisplayMode displayMode,
                                const Data::View &view,
                                unsigned int w, unsigned int h)
{
  m_font.setPointSize(32);
  m_font.setPenAlpha(0.75f);
  
  // active / inactive color
  const float aColor[3] = { 1.0f, 1.0f, 1.0f };
  const float iColor[3] = { 0.5f, 0.5f, 0.5f };
  
  const unsigned int xPos = 16u;
  const unsigned int yPos = 32u;
  const unsigned int halfWidth = 0.5f*w;
  
    
  if (displayMode == Data::UI::DISPLAY_SPLITTED)
  {
    // main cam
    if (view.active == Data::View::CAMERA_MAIN) {
      m_font.setPenColor(aColor[0], aColor[1], aColor[2]);
    } else {
      m_font.setPenColor(iColor[0], iColor[1], iColor[2]);
    }
    
    m_font.setPenPosition( xPos, yPos);
    m_font.draw("MAIN");
    
    // debug cam
    if (view.active == Data::View::CAMERA_DEBUG) {
      m_font.setPenColor(aColor[0], aColor[1], aColor[2]);
    } else {
      m_font.setPenColor(iColor[0], iColor[1], iColor[2]);
    }
    
    m_font.setPenPosition( xPos + halfWidth, yPos);
    m_font.draw("DEBUG");
  }
  else
  {
    // active cam
    m_font.setPenPosition( xPos, yPos);
    m_font.setPenColor(aColor[0], aColor[1], aColor[2]);
    m_font.draw((view.active == Data::View::CAMERA_MAIN) ? "MAIN" : "DEBUG");
  }
}

} // namespace app
