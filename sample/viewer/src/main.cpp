///
///
///

#include <cstdlib>
#include "Viewer.hpp"


int main(int argc, char *argv[])
{
  app::Viewer viewer;
  
  viewer.init( argc, argv);  
  viewer.run();
  
  return EXIT_SUCCESS;
}
