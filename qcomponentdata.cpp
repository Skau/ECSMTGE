#include "qcomponentdata.h"
#include "resourcemanager.h"

void QMeshComponent::setMesh(const QString& name)
{
    auto mesh = ResourceManager::instance()->getMesh(name.toStdString());
    if(mesh)
    {        
        mComp.meshData = *mesh;
        mComp.isVisible = true;
    }
}

void QMeshComponent::setShader(const QString& name)
{
    auto shader = ResourceManager::instance()->getShader(name.toStdString());
    if(shader)
    {
        mComp.mMaterial.loadShaderWithParameters(shader);
    }
}
