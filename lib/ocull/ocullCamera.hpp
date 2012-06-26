///
///
///
///

#ifndef OCULL_CAMERA_HPP_
#define OCULL_CAMERA_HPP_

#include "ocullDefs.hpp"


namespace ocull {

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

  Camera( const Frustum& frustum, const Matrix4x4& viewMatrix);
};

} //namespace ocull

#endif //OCULL_CAMERA_HPP_
