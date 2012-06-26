///
///
///
///

#ifndef OCULL_CAMERA_HPP_
#define OCULL_CAMERA_HPP_

#include "ocullDefs.hpp"


namespace ocull {

// TODO assert projMatrix match the depth buffer resolution (right aspect ratio)
struct Frustum 
{
  float left;
  float right;
  float bottom;
  float top;  
  float near;
  float far;
  
  Matrix4x4 projectionMatrix;
  
  //--
  Frustum() 
      : left(-1.0f), right(1.0f), bottom(-1.0f), top(1.0f), 
        near(0.1f), far(1000.0f)
  {}
  
  Frustum(float fov, float aspectRatio, float zNear, float zFar)
  {
  }
  
  Frustum(float left, float right, float bottom, float top, 
          float zNear, float zFar)
  {
  }
};


// Structure to holds depth buffer output.
// TODO redefines more precisely
struct DepthBuffer 
{
  struct Desc {
    unsigned int width;
    unsigned int height;
    //Format format;
  } desc;
  
  void *depth;
};


struct Camera 
{
  Frustum frustum;
  Matrix4x4 viewMatrix;
};

} //namespace ocull

#endif //OCULL_CAMERA_HPP_
