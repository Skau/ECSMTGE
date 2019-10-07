#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>
#include <memory>

#include "texture.h"

#include "Renderables/vertex.h"

#include "meshdata.h"

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#elif __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#include "wavfilehandler.h"

class SoundSource;

class ResourceManager : protected QOpenGLFunctions_4_1_Core
{
public:
    static ResourceManager* instance()
    {
        static ResourceManager instance;
        return &instance;
    }

    virtual ~ResourceManager();

    /**
     * @brief Iteratively goes through all folders in Assets/ and loads the files.
     */
    void LoadAssetFiles();

    void addShader(const std::string& name, std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> getShader(const std::string& name);

    void addTexture(const std::string& name, const std::string& path);
    void addCubemapTexture(const std::string& name, const std::string& path);
    int getTexture(const std::string& name);

    std::shared_ptr<MeshData> addMesh(const std::string& name, const std::string& path, GLenum renderType = GL_TRIANGLES);
    std::shared_ptr<MeshData> getMesh(const std::string& name);

    void loadWav(const std::string& name, const std::string& path);
    wave_t* getWav(const std::string& name);

    int getSourceBuffer(const std::string& name);

    std::vector<std::string> getAllMeshNames();
    std::vector<std::string> getAllShaderNames();
    std::vector<std::string> getAllTextureNames();
    std::vector<std::string> getAllWavFileNames();

private:
    ResourceManager();

    std::pair<std::vector<Vertex>, std::vector<GLuint>> readObjFile(std::string filename, bool relative = true);
    std::pair<std::vector<Vertex>, std::vector<GLuint>> readTxtFile(std::string filename);

    /**
     * @brief Creates LOD meshes for a given data pair. Only works with OBJ files. Hardcoded to create 2 LODs
     */
    std::pair<std::shared_ptr<MeshData>, std::shared_ptr<MeshData> > initializeLODs(std::pair<std::vector<Vertex>, std::vector<GLuint> > data, const std::string& name, const std::string& path, GLenum renderType);
    void setupLODs(std::shared_ptr<MeshData> baseMeshData, std::shared_ptr<MeshData> LOD1 = nullptr, std::shared_ptr<MeshData> LOD2 = nullptr);

    std::shared_ptr<MeshData> initializeMeshData(const std::string& name, GLenum renderType, std::pair<std::vector<Vertex>, std::vector<GLuint>> data);

    std::map<std::string, std::shared_ptr<Shader>> mShaders;
    std::map<std::string, std::shared_ptr<Texture>> mTextures;
    std::map<std::string, std::shared_ptr<MeshData>> mMeshes;
    std::map<std::string, wave_t*> mWavFiles;
    std::map<std::string, unsigned int> mSourceBuffers;

    bool mIsInitialized = false;
};

#endif // RESOURCEMANAGER_H
