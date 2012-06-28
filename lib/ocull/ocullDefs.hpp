///
///   \file ocullDefs.hpp
///
///   Algebraic structures & usefuls definitions
///

#ifndef OCULL_DEFS_HPP_
#define OCULL_DEFS_HPP_

#include <glm/glm.hpp>


namespace ocull {

#if 1

typedef glm::vec2  Vector2;
typedef glm::ivec2 Vector2i;
typedef glm::vec3  Vector3;
typedef glm::ivec3 Vector3i;
typedef glm::vec4  Vector4;
typedef glm::mat4  Matrix4x4;

#else

/// Vector types can be overrided by defining OCULL_OVERRIDE_VECTOR
/// before including any Ocull headers and using typedefs while the
/// underlying data layout is respected.
#ifndef OCULL_OVERRIDE_VECTOR
class Vector2
{
  float x, y;
};

class Vector2i
{
  int x, y;
};

class Vector3
{
  float x, y, z;
};
  
class Vector3i
{
  int x, y, z;
};

class Vector4
{
  float x, y, z, w;
};

class Matrix4x4
{
  float m[4][4]; // column major matrix
};
#endif // OCULL_OVERRIDE_VECTOR

#endif

} // namespace ocull


/// A macro to disallow the copy constructor and operator= functions
/// This should be used in the private: declarations for a class
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)
#endif // DISALLOW_COPY_AND_ASSIGN


#ifndef OCULL_SAFEDELETE
#define OCULL_SAFEDELETE(x) if (x) { delete x; x = NULL; }
#endif //OCULL_SAFEDELETE


#endif // OCULL_DEFS_HPP_
