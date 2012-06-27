///
///   \file ocullCamera.hpp
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
  float zNear;
  float zFar; 
  
  Matrix4x4 projectionMatrix; 
  
  
  // +++ Constructor +++
  
  Frustum() 
      : Frustum( -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1000.0f)
  {}
    
  Frustum(float fov_, float aspectRatio_, float zNear_, float zFar_)
      : zNear(zNear_),
        zFar(zFar_)
  {
    projectionMatrix = glm::perspective( fov_, aspectRatio_, zNear, zFar);      // ~ xxx
    
    // TODO set l, r, b, t
  }
  
  Frustum(float left_, float right_, float bottom_, float top_, 
          float zNear_, float zFar_)
      : left(left_), 
        right(right_), 
        bottom(bottom_), 
        top(top_),
        zNear(zNear_), 
        zFar(zFar_)
  {
    projectionMatrix = glm::frustum( left, right, bottom, top, zNear, zFar);    // ~ xxx
  }  
};


class Camera
{
  private:
    Frustum   m_frustum;
    Matrix4x4 m_viewMatrix;
    Matrix4x4 m_viewProjMatrix;
    bool      m_bRebuild;
  
  public:
    Camera() : m_bRebuild(true) {}
    
    
    // +++ Setters +++
    
    inline void setFrustum(const Frustum& frustum)
    {
      m_frustum = frustum;
      m_bRebuild = true;
    }
    
    inline void setViewMatrix(const Matrix4x4& viewMatrix)
    {
      m_viewMatrix = viewMatrix;
      m_bRebuild = true;
    }
    
    
    // +++ Getters +++
    
    inline const Frustum& getFrustum() const { return m_frustum; }
    
    inline const Matrix4x4& getViewMatrix() const { return m_viewMatrix; }
    
    inline const Matrix4x4& getViewProjMatrix() const
    {
      if (m_bRebuild)
      { 
        m_viewProjMatrix = m_frustum.projectionMatrix * m_viewMatrix;
        m_bRebuild = false;
      }
      return m_viewProjMatrix;
    }
};

} //namespace ocull

#endif //OCULL_CAMERA_HPP_
