#version 330 core

layout(location = 0) in vec3 posAttr;
layout(location = 1) in vec3 colAttr;
out vec3 col;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main() {
   col = abs(colAttr);
   mat4 viewMat = mat4(mat3(vMatrix));
   viewMat[3].z = -5.0;
   mat4 UIMat = mat4(mat3(0.3));
   UIMat[3].xy = vec2(-0.9, -0.9);
   gl_Position = UIMat * pMatrix * viewMat * mMatrix * vec4(posAttr, 1);
}
