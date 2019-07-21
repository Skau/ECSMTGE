#include "innpch.h"
#include "phongshader.h"
#include "material.h"
#include "light.h"

PhongShader::PhongShader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath)
    :Shader(vertexPath, fragmentPath, geometryPath)
{
    mMatrixUniform = glGetUniformLocation( program, "mMatrix" );
    vMatrixUniform = glGetUniformLocation( program, "vMatrix" );
    pMatrixUniform = glGetUniformLocation( program, "pMatrix" );

    //    textureUniform = glGetUniformLocation(program, "textureSampler");
    mLightColorUniform = glGetUniformLocation( program, "lightColor" );
    mObjectColorUniform = glGetUniformLocation( program, "objectColor" );
    mAmbientLightStrengthUniform = glGetUniformLocation( program, "ambientStrengt" );
    mLightPositionUniform = glGetUniformLocation( program, "lightPosition" );
    mSpecularStrengthUniform = glGetUniformLocation( program, "specularStrength" );
    mSpecularExponentUniform = glGetUniformLocation( program, "specularExponent" );
    mLightPowerUniform = glGetUniformLocation( program, "lightPower" );
    mCameraPositionUniform = glGetUniformLocation( program, "cameraPosition" );
}

void PhongShader::transmitUniformData(gsl::Matrix4x4 *modelMatrix, Material *material)
{
    Shader::transmitUniformData(modelMatrix);

    //    glUniform1i(textureUniform, material->mTextureUnit); //TextureUnit = 0 as default);
    glUniform1f(mAmbientLightStrengthUniform, mLight->mAmbientStrenght);
    glUniform1f(mLightPowerUniform, mLight->mLightStrenght);
    glUniform3f(mLightColorUniform, mLight->mLightColor.x, mLight->mLightColor.y, mLight->mLightColor.z);
    glUniform3f(mLightPositionUniform, mLight->mMatrix.getPosition().x, mLight->mMatrix.getPosition().y, mLight->mMatrix.getPosition().x);
    glUniform3f(mObjectColorUniform, material->mObjectColor.x, material->mObjectColor.y, material->mObjectColor.z);
}

void PhongShader::setLight(Light *light)
{
    mLight = light;
}
