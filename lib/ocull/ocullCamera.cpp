///
///   \file ocullCamera.cpp
///
///

#include "ocullCamera.hpp"


namespace Ocull {

Frustum::Frustum(float fov, float aspectRatio, float zNear, float zFar)
{ 
}

Camera::Camera( const Frustum& frustum, const Ocull::Matrix4x4& viewMatrix)
    : m_frustum(frustum),
      m_viewMatrix(viewMatrix)
{
}

}

