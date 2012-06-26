/*
 *          PassThrough.glsl
 *
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;

// OUT
out vec3 vNormal;

// UNIFORM
uniform mat4 uModelViewProjMatrix;


void main()
{
  gl_Position = uModelViewProjMatrix * inPosition;
  vNormal = inNormal;//
}


--

//------------------------------------------------------------------------------


-- Fragment

// IN
in vec3 vNormal;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform vec3 uColor = vec3( 1.0f, 0.0f, 0.0f);
uniform bool uEnableLight = true;

void main()
{
  float diffuse = 1.0f;

  // Simple diffuse light model
  if (uEnableLight)
  {
    const vec3 lightDirection = normalize(vec3(-1.0f, 1.0f, 1.0f));
    
    vec3 normal = (gl_FrontFacing) ? vNormal : -vNormal;
         normal = normalize(normal);  
   
    diffuse = max( 0.1f, dot( normal, lightDirection));
  }
  
  fragColor = vec4( diffuse*uColor, 1.0f);
}

