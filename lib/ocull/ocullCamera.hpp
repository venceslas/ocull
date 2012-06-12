///
///   \file ocullCamera.hpp
///
///

#ifndef OCULL_CAMERA_HPP_
#define OCULL_CAMERA_HPP_

#include "ocullDefs.hpp"


namespace Ocull {

///
///
///
struct Frustum
{
  public:
    float left;
    float right;
    float top;
    float bottom;
    float zNear;
    float zFar;
  
  public:
    Frustum(float fov, float aspectRatio, float zNear, float zFar);
};

///
///
///
class Camera
{
  private:
    Frustum m_frustum;
    Matrix4x4 m_viewMatrix;
    
  public:
    Camera( const Frustum& frustum, const Ocull::Matrix4x4& viewMatrix);
    
    const Frustum& getFrustum() const;
    const Ocull::Matrix4x4& getViewMatrix() const;
    
    void setFrustum( const Frustum& frustum );    
    void setViewMatrix( const Ocull::Matrix4x4& viewMatrix );
};

}

#endif // OCULL_CAMERA_HPP_
