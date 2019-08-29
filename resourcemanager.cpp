#include "resourcemanager.h"

#include <QDebug>

ResourceManager::~ResourceManager()
{
    for(auto& shader : mShaders)
    {
        delete shader.second;
    }

    for(auto& texture : mTextures)
    {
        delete texture.second;
    }
}

void ResourceManager::addShader(const std::string &name, Shader *shader)
{
    if(shader && mShaders.find(name) == mShaders.end())
    {
        mShaders[name] = shader;

        qDebug() << "ResourceManager: Added shader " << QString::fromStdString(name) << " (id: " << shader->getProgram() << ")";
    }
}

Shader *ResourceManager::getShader(const std::string &name)
{
    if(mShaders.find(name) != mShaders.end())
    {
        return mShaders[name];
    }

    return nullptr;
}

void ResourceManager::loadTexture(const std::string &name, const std::string &path)
{
    if(mTextures.find(name) == mTextures.end())
    {
        if(!mIsInitialized)
        {
            initializeOpenGLFunctions();
            mIsInitialized = true;
        }

        mTextures[name] = new Texture(path);

        glActiveTexture(static_cast<GLuint>(GL_TEXTURE0 + mTextures.size() - 1));
        glBindTexture(GL_TEXTURE_2D, mTextures[name]->id());
    }
}

Texture *ResourceManager::getTexture(const std::string &name)
{
    if(mTextures.find(name) != mTextures.end())
    {
        return mTextures[name];
    }

    return nullptr;
}

ResourceManager::ResourceManager(){}
