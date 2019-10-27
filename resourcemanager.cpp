#include "resourcemanager.h"
#include "innpch.h"
#include <QDirIterator>
#include "constants.h"
#include "wavfilehandler.h"
#include <QDebug>
#include "soundmanager.h"

ResourceManager::~ResourceManager()
{

}

void ResourceManager::LoadAssetFiles()
{
    QDirIterator dirIt(QString::fromStdString(gsl::assetFilePath), QDirIterator::Subdirectories);
    while (dirIt.hasNext())
    {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
        {
            auto fileInfo = QFileInfo(dirIt.filePath());
            auto baseName = fileInfo.baseName().toStdString();
            auto fileName = fileInfo.fileName().toStdString();
            if (fileInfo.suffix() == "txt" || fileInfo.suffix() == "obj")
            {
                addMesh(baseName, fileName);
            }
            else if(fileInfo.suffix() == "bmp")
            {
                addTexture(baseName, fileName);
            }
            else if(fileInfo.suffix() == "wav")
            {
                loadWav(baseName, fileInfo.absoluteFilePath().toStdString());
            }
        }
    }
}

void ResourceManager::addShader(const std::string &name, std::shared_ptr<Shader> shader)
{
    if(shader && mShaders.find(name) == mShaders.end())
    {
        shader->mName = name;
        mShaders[name] = shader;

        //qDebug() << "ResourceManager: Added shader " << QString::fromStdString(name) << " (id: " << shader->getProgram() << ")";
    }
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string &name)
{
    if(mShaders.find(name) != mShaders.end())
    {
        return mShaders[name];
    }

    return nullptr;
}

void ResourceManager::addTexture(const std::string &name, const std::string &path, GLenum type)
{
    if(mTextures.find(name) == mTextures.end())
    {
        if(!mIsInitialized)
        {
            initializeOpenGLFunctions();
            mIsInitialized = true;
        }

        mTextures[name] = std::make_shared<Texture>(path, type);
    }
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string &name)
{
    if(mTextures.find(name) != mTextures.end())
    {
        return mTextures[name];
    }

    return nullptr;
}

std::shared_ptr<MeshData> ResourceManager::addMesh(const std::string& name, const std::string& path, GLenum renderType)
{
    if(mMeshes.find(name) == mMeshes.end())
    {
        if(!mIsInitialized)
        {
            initializeOpenGLFunctions();
            mIsInitialized = true;
        }

        if(QString::fromStdString(path).contains(".obj"))
        {
            auto verticesIndicesPair = readObjFile(path);
            if(!verticesIndicesPair.first.size()) return nullptr;
            auto mesh = initializeMeshData(name, renderType, verticesIndicesPair);
            auto [LOD1, LOD2] = initializeLODs(name, path, renderType);
            setupLODs(mesh, LOD1, LOD2);
            return mesh;
        }
        else
        {
            auto verticesIndicesPair = readTxtFile(path);
            if(!verticesIndicesPair.first.size()) return nullptr;
            auto mesh = initializeMeshData(name, renderType, verticesIndicesPair);
            setupLODs(mesh);
            return mesh;

        }
    }

    return nullptr;
}

std::pair<std::shared_ptr<MeshData>, std::shared_ptr<MeshData>> ResourceManager::initializeLODs(const std::string& name, const std::string& path, GLenum renderType)
{
    auto split = QString::fromStdString(path).split(".");
    auto firstSplit = split[0].toStdString();

    std::shared_ptr<MeshData> LOD1{nullptr};
    std::shared_ptr<MeshData> LOD2{nullptr};

    // LOD1
    auto LOD1Name = name + "_L01.obj";
    auto verticesIndicesPair = readObjFile(LOD1Name);
    if(verticesIndicesPair.first.size())
    {
        LOD1 = initializeMeshData(LOD1Name, renderType, verticesIndicesPair);

        // LOD2
        auto LOD2Name = name + "_L02.obj";
        verticesIndicesPair = readObjFile(LOD2Name);
        if(verticesIndicesPair.first.size())
        {
            LOD2 = initializeMeshData(LOD2Name, renderType, verticesIndicesPair);
        }
    }

    return {LOD1, LOD2};
}

void ResourceManager::setupLODs(std::shared_ptr<MeshData> baseMeshData, std::shared_ptr<MeshData> LOD1, std::shared_ptr<MeshData> LOD2)
{
    if(LOD1)
    {
        baseMeshData->mVAOs[1]              = LOD1->mVAOs[0];
        baseMeshData->mVerticesCounts[1]    = LOD1->mVerticesCounts[0];
        baseMeshData->mIndicesCounts[1]     = LOD1->mIndicesCounts[0];

        if(LOD2)
        {
            baseMeshData->mVAOs[2]              = LOD2->mVAOs[0];
            baseMeshData->mVerticesCounts[2]    = LOD2->mVerticesCounts[0];
            baseMeshData->mIndicesCounts[2]     = LOD2->mIndicesCounts[0];
            mMeshes.erase(LOD2->mName);
        }
        else
        {
            baseMeshData->mVAOs[2]              = LOD1->mVAOs[0];
            baseMeshData->mVerticesCounts[2]    = LOD1->mVerticesCounts[0];
            baseMeshData->mIndicesCounts[2]     = LOD1->mIndicesCounts[0];
        }

        mMeshes.erase(LOD1->mName);
    }
    else
    {
        baseMeshData->mVAOs[1]              = baseMeshData->mVAOs[0];
        baseMeshData->mVerticesCounts[1]    = baseMeshData->mVerticesCounts[0];
        baseMeshData->mIndicesCounts[1]     = baseMeshData->mIndicesCounts[0];
        baseMeshData->mVAOs[2]              = baseMeshData->mVAOs[0];
        baseMeshData->mVerticesCounts[2]    = baseMeshData->mVerticesCounts[0];
        baseMeshData->mIndicesCounts[2]     = baseMeshData->mIndicesCounts[0];
    }
}

std::shared_ptr<MeshData> ResourceManager::initializeMeshData(const std::string& name, GLenum renderType, std::pair<std::vector<Vertex>, std::vector<GLuint>> data)
{
    MeshData meshData;

    meshData.mName = name;
    meshData.mRenderType = renderType;

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &meshData.mVAOs[0] );
    glBindVertexArray(meshData.mVAOs[0]);

    //Vertex Buffer Object to hold vertices - VBO
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizei>(data.first.size() * sizeof(Vertex)), data.first.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)( 6 * sizeof( GLfloat ) ));
    glEnableVertexAttribArray(2);

    meshData.mVerticesCounts[0] = static_cast<unsigned>(data.first.size());
    meshData.mIndicesCounts[0] = static_cast<unsigned>(data.second.size());
    meshData.bounds = CalculateBounds(data.first);

    if(meshData.mIndicesCounts[0])
    {
        //Second buffer - holds the indices (Element Array Buffer - EAB):
        GLuint eab;
        glGenBuffers(1, &eab);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.mIndicesCounts[0] * sizeof(GLuint), data.second.data(), GL_STATIC_DRAW);
    }

    auto mesh = std::make_shared<MeshData>(meshData);
    mMeshes[name] = mesh;
    glBindVertexArray(0);

    return mesh;
}

MeshData::Bounds ResourceManager::CalculateBounds(const std::vector<Vertex> &vertices)
{
    float minx{0.f}, miny{0.f}, minz{0.f},
    maxx{0.f}, maxy{0.f}, maxz{0.f};

    for (auto vertex : vertices)
    {
        minx = (vertex.get_xyz().x < minx) ? vertex.get_xyz().x : minx;
        miny = (vertex.get_xyz().y < miny) ? vertex.get_xyz().y : miny;
        minz = (vertex.get_xyz().z < minz) ? vertex.get_xyz().z : minz;

        maxx = (maxx < vertex.get_xyz().x) ? vertex.get_xyz().x : maxx;
        maxy = (maxy < vertex.get_xyz().y) ? vertex.get_xyz().y : maxy;
        maxz = (maxz < vertex.get_xyz().z) ? vertex.get_xyz().z : maxz;
    }

    MeshData::Bounds b;
    b.centre.x = (minx + maxx) / 2.0f;
    b.centre.y = (miny + maxy) / 2.0f;
    b.centre.z = (minz + maxz) / 2.0f;

    b.radius = static_cast<float>(std::sqrt(std::pow((maxx - b.centre.x), 2) + std::pow((maxy - b.centre.y), 2) + std::pow((maxz - b.centre.z), 2)));
    return b;
}

std::shared_ptr<MeshData> ResourceManager::getMesh(const std::string& name)
{
    if(mMeshes.find(name) != mMeshes.end())
    {
        return mMeshes[name];
    }
    qDebug() << "Error ResourceManager: No mesh named " << QString::fromStdString(name) << " could be found";
    return nullptr;
}

void ResourceManager::loadWav(const std::string& name, const std::string& path)
{
    auto waveData = new wave_t();
    if (!WavFileHandler::loadWave(path, waveData))
    {
        qDebug() << "Error ResourceManager: Failed loading wave file " << QString::fromStdString(name) << "!";
        return;
    }

    std::ostringstream i2s;
    i2s << waveData->dataSize;
    mWavFiles[name] = waveData;
}

wave_t* ResourceManager::getWav(const std::string& name)
{
    if(mWavFiles.find(name) != mWavFiles.end())
    {
        return mWavFiles[name];
    }

    return nullptr;
}

int ResourceManager::getSourceBuffer(const std::string& name)
{
    if(mSourceBuffers.find(name) != mSourceBuffers.end())
    {
        return static_cast<int>(mSourceBuffers[name]);
    }

    return -1;
}

std::vector<std::string> ResourceManager::getAllMeshNames()
{
    std::vector<std::string> returnStrings;

    for(auto& mesh : mMeshes)
    {
        returnStrings.push_back(mesh.first);
    }

    return returnStrings;
}

std::vector<std::string> ResourceManager::getAllShaderNames()
{
    std::vector<std::string> returnStrings;

    for(auto& shader : mShaders)
    {
        returnStrings.push_back(shader.first);
    }

    return returnStrings;
}

std::vector<std::string> ResourceManager::getAllTextureNames()
{
    std::vector<std::string> returnStrings;

    for(auto& texture : mTextures)
    {
        returnStrings.push_back(texture.first);
    }

    return returnStrings;
}

std::vector<std::string> ResourceManager::getAllWavFileNames()
{
    std::vector<std::string> returnStrings;

    for(auto& wav : mWavFiles)
    {
        returnStrings.push_back(wav.first);
    }

    return returnStrings;
}

std::pair<std::vector<Vertex>, std::vector<GLuint>> ResourceManager::readObjFile(std::string filename, bool relative)
{
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;

    //Open File

    std::ifstream fileIn;
    if(relative)
    {
        std::string fileWithPath = gsl::assetFilePath + "Meshes/" + filename;
        fileIn.open (fileWithPath, std::ifstream::in);
    }
    else
    {
        fileIn.open(filename, std::ifstream::in);
    }

    if(!fileIn)
    {
        qDebug() << "Error ResourceManager: Could not open file for reading: " << QString::fromStdString(filename);
        return {};
    }

    //One line at a time-variable
    std::string oneLine;
    //One word at a time-variable
    std::string oneWord;

    std::vector<gsl::Vector3D> tempVertecies;
    std::vector<gsl::Vector3D> tempNormals;
    std::vector<gsl::Vector2D> tempUVs;

    //    std::vector<Vertex> mVertices;    //made in VisualObject
    //    std::vector<GLushort> mIndices;   //made in VisualObject

    // Varible for constructing the indices vector
    unsigned int temp_index = 0;

    bool normalsPresent = false;
    bool UVsPresent = false;

    //Reading one line at a time from file to oneLine
    while(std::getline(fileIn, oneLine))
    {
        //Doing a trick to get one word at a time
        std::stringstream sStream;
        //Pushing line into stream
        sStream << oneLine;
        //Streaming one word out of line
        oneWord = ""; //resetting the value or else the last value might survive!
        sStream >> oneWord;

        if (oneWord == "#")
        {
            //Ignore this line
            //            qDebug() << "Line is comment "  << QString::fromStdString(oneWord);
            continue;
        }
        if (oneWord == "")
        {
            //Ignore this line
            //            qDebug() << "Line is blank ";
            continue;
        }
        if (oneWord == "v")
        {
            //            qDebug() << "Line is vertex "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector3D tempVertex;
            sStream >> oneWord;
            tempVertex.x = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.y = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempVertecies.push_back(tempVertex);

            continue;
        }
        if (oneWord == "vt")
        {
            //            qDebug() << "Line is UV-coordinate "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector2D tempUV;
            sStream >> oneWord;
            tempUV.x = std::stof(oneWord);
            sStream >> oneWord;
            tempUV.y = std::stof(oneWord);

            //UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            UVsPresent = true;
            continue;
        }
        if (oneWord == "vn")
        {
            //            qDebug() << "Line is normal "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector3D tempNormal;
            sStream >> oneWord;
            tempNormal.x = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.y = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempNormals.push_back(tempNormal);

            normalsPresent = true;
            continue;
        }
        if (oneWord == "f")
        {
            //            qDebug() << "Line is a face "  << QString::fromStdString(oneWord) << " ";
            //int slash; //used to get the / from the v/t/n - format
            int index, normal, uv;
            for(int i = 0; i < 3; i++)
            {
                sStream >> oneWord;     //one word read
                std::stringstream tempWord(oneWord);    //to use getline on this one word
                std::string segment;    //the numbers in the f-line
                std::vector<std::string> segmentArray;  //temp array of the numbers
                while(std::getline(tempWord, segment, '/')) //splitting word in segments
                {
                    segmentArray.push_back(segment);
                }

                index = std::stoi(segmentArray[0]);
                --index;

                if(UVsPresent)
                {
                    uv = std::stoi(segmentArray[1]);
                    --uv;
                }

                if(normalsPresent)
                {
                    normal = std::stoi(segmentArray[2]);
                    --normal;
                }

                Vertex tempVert(tempVertecies[index]);

                if (UVsPresent)
                {
                    tempVert.set_uv(tempUVs[uv].x, tempUVs[uv].y);
                }
                else
                {
                    tempVert.set_uv(0.f, 0.f);
                }


                if(normalsPresent)
                {
                    tempVert.set_normal(tempNormals[normal]);
                }
                else
                {
                    tempVert.set_normal(0, 1, 0);
                }

                mVertices.push_back(tempVert);
                mIndices.push_back(temp_index++);
            }

            //For some reason the winding order is backwards so fixing this by swapping the last two indices
            //Update: this was because the matrix library was wrong - now it is corrected so this is no longer needed.
//            unsigned int back = mIndices.size() - 1;
//            std::swap(mIndices.at(back), mIndices.at(back-1));
            continue;
        }
    }

    //beeing a nice boy and closing the file after use
    fileIn.close();

    return {mVertices, mIndices};
}

std::pair<std::vector<Vertex>, std::vector<GLuint>> ResourceManager::readTxtFile(std::string filename)
{
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;

    std::ifstream inn;
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + filename;

    inn.open(fileWithPath);

    if (inn.is_open()) {
        int n;
        Vertex vertex;
        inn >> n;
        mVertices.reserve(n);
        for (int i=0; i<n; i++) {
            inn >> vertex;
            mVertices.push_back(vertex);
        }
        inn.close();
    }
    else
    {
        qDebug() << "Error ResourceManager: Could not open file for reading: " << QString::fromStdString(filename);
    }

    return {mVertices, mIndices};
}

ResourceManager::ResourceManager(){}
