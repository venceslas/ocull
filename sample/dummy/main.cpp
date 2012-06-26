///
///   This sample file was just for the lib to compile (to test minor code error)
///

#include <cstdio>
#include <cstdlib>

#include "ocull/ocull.hpp"


int main( int argc, char *argv[])
{
  ocull::Context context;
  
  // Not really clean to call init() for the context object (just after creating it)
  context.init(); 
  
  ocull::Query *query = new ocull::Query( context );
  
  
  return EXIT_SUCCESS;
}
