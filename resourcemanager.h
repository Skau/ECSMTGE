#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>

#include "Shaders/colorshader.h"
#include "Shaders/textureshader.h"
#include "Shaders/phongshader.h"

#include "texture.h"

class ResourceManager : protected QOpenGLFunctions_4_1_Core
{
public:
    static ResourceManager* instance()
    {
        static ResourceManager instance;
        return &instance;
    }

    virtual ~ResourceManager();

    void addShader(const std::string& name, Shader* shader);
    Shader* getShader(const std::string& name);

    void loadTexture(const std::string& name, const std::string& path);
    Texture* getTexture(const std::string& name);

private:
    ResourceManager();

    std::map<std::string, Shader*> mShaders;
    std::map<std::string, Texture*> mTextures;

    bool mIsInitialized = false;
};

#endif // RESOURCEMANAGER_H
