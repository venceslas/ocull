///
///     UserInterface.hpp
///
///   Application's user interface (mostly text display)
///

#ifndef APP_USERINTERFACE_HPP__
#define APP_USERINTERFACE_HPP__

#include <gltext/gltext.hpp>
#include "Data.hpp"

namespace app {

class UserInterface
{
  private:
    bool m_bInitialized;
    gltext::Font m_font;
    
  public:
    UserInterface() : m_bInitialized(false) {}
    
    void init();    
    void render(const Data &data);
  
  private:
    void displayStats(const Data::Stats &stats, unsigned int w, unsigned int h);
    
    void displayView(Data::UI::DisplayMode displayMode, const Data::View &view,
                     unsigned int w, unsigned int h);
};

} //namespace app

#endif //APP_USERINTERFACE_HPP__
