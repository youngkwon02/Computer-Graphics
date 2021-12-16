#version 430 core

// Attributes passed on from the vertex shader
smooth in vec3 FragmentPosition;
smooth in vec3 FragmentNormal;
smooth in vec2 FragmentTexCoord;
in vec4 eyePos;

/// @brief our output fragment colour
layout (location=0) out vec4 FragColour;

uniform samplerCube envMap;
uniform mat4 invV;


void main ()
{
    // Calculate the eye vector
    vec3 v = normalize(vec3(-FragmentPosition));

    vec3 p = -FragmentPosition;

    vec3 lookup = normalize(eyePos.xyz / eyePos.w);
    lookup.y *= -1;
    lookup.z *= -1;

    vec4 colour = texture(envMap, lookup);

    FragColour = colour;
    //FragColour = vec4(lookup, 1);
}

