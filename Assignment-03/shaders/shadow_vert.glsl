#version 430 core

layout(location = 0) in vec3 VertexPosition;

uniform mat4 depthMVP;

void main()
{
 gl_Position = depthMVP * vec4(VertexPosition, 1.f);
}
