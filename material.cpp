#include "innpch.h"
#include "material.h"
#include "Shaders/textureshader.h"

MaterialClass::MaterialClass()
{

}

void MaterialClass::setColor(const gsl::Vector3D &color)
{
    mObjectColor = color;
}

void MaterialClass::setTextureUnit(const GLuint &textureUnit)
{
    mTextureUnit = textureUnit;
}

void MaterialClass::setShader(Shader *shader)
{
    mShader = shader;
}
