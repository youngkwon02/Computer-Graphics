#version 430 core

layout(location = 0) out vec4 fragmentdepth;

void main()
{
    fragmentdepth = vec4(gl_FragCoord.z);
}
