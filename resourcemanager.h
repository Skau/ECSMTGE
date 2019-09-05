#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>
#include <memory>

#include "Shaders/colorshader.h"
#include "Shaders/textureshader.h"
#include "Shaders/phongshader.h"

#include "texture.h"

#include "Renderables/vertex.h"

#include "meshdata.h"


class ResourceManager : protected QOpenGLFunctions_4_1_Core
{
public:
    static ResourceManager* instance()
    {
        static ResourceManager instance;
        return &instance;
    }

    virtual ~ResourceManager();

    void addShader(const std::string& name, std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> getShader(const std::string& name);

    void loadTexture(const std::string& name, const std::string& path);
    Texture* getTexture(const std::string& name);

    void addMesh(const std::string& name, const std::string& path, GLenum renderType = GL_TRIANGLES);
    std::optional<MeshData> getMesh(const std::string& name);


private:
    ResourceManager();

    std::pair<std::vector<Vertex>, std::vector<GLuint> > readObjFile(std::string filename);
    std::pair<std::vector<Vertex>, std::vector<GLuint>> readTxtFile(std::string filename);

    std::map<std::string, std::shared_ptr<Shader>> mShaders;
    std::map<std::string, Texture*> mTextures;
    std::map<std::string, MeshData> mMeshes;

    bool openglInitialized{false};

    bool mIsInitialized = false;
};

#endif // RESOURCEMANAGER_H
