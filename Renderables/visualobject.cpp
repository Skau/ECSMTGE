#include "innpch.h"
#include "visualobject.h"


VisualObject::VisualObject()
{
}

VisualObject::~VisualObject()
{
   glDeleteVertexArrays( 1, &mVAO );
   glDeleteBuffers( 1, &mVBO );
}

void VisualObject::init()
{
}

void VisualObject::setShader(std::shared_ptr<Shader> shader)
{
    mMaterial.mShader = shader;
}
