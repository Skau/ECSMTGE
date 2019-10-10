#version 330 core

layout(location = 0) in vec3 posAttr;
layout(location = 1) in vec3 colAttr;

uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main() 
{
   gl_Position = pMatrix * vMatrix * mMatrix * vec4(posAttr, 1);
}
