#include "resourcemanager.h"
#include "innpch.h"
#include "wavfilehandler.h"
#include <QDebug>
#include "openalmanager.h"
#include "soundsource.h"

ResourceManager::~ResourceManager()
{

}

void ResourceManager::addShader(const std::string &name, std::shared_ptr<Shader> shader)
{
    if(shader && mShaders.find(name) == mShaders.end())
    {
        shader->mName = name;
        mShaders[name] = shader;

        qDebug() << "ResourceManager: Added shader " << QString::fromStdString(name) << " (id: " << shader->getProgram() << ")";
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

void ResourceManager::addTexture(const std::string &name, const std::string &path)
{
    if(mTextures.find(name) == mTextures.end())
    {
        if(!mIsInitialized)
        {
            initializeOpenGLFunctions();
            mIsInitialized = true;
        }

        mTextures[name] = std::make_shared<Texture>(path);
    }
}

int ResourceManager::getTexture(const std::string &name)
{
    if(mTextures.find(name) != mTextures.end())
    {
        return static_cast<int>(mTextures[name]->id());
    }

    return -1;
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

        std::pair<std::vector<Vertex>, std::vector<GLuint>> data;
        if(QString::fromStdString(path).contains(".obj"))
        {
            data = readObjFile(path);
        }
        else
        {
            data = readTxtFile(path);
        }

        if(!data.first.size()) return nullptr;

        MeshData meshData;

        meshData.mName = name;
        meshData.mRenderType = renderType;

        //Vertex Array Object - VAO
        glGenVertexArrays( 1, &meshData.mVAO );
        glBindVertexArray(meshData.mVAO);

        //Vertex Buffer Object to hold vertices - VBO
        GLuint vbo;
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );

        glBufferData( GL_ARRAY_BUFFER, data.first.size() * sizeof(Vertex), data.first.data(), GL_STATIC_DRAW );

        // 1rst attribute buffer : vertices
        glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // 2nd attribute buffer : colors
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
        glEnableVertexAttribArray(1);

        // 3rd attribute buffer : uvs
        glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)( 6 * sizeof( GLfloat ) ));
        glEnableVertexAttribArray(2);

        meshData.mVerticesCount = data.first.size();
        meshData.mIndicesCount = data.second.size();

        if(meshData.mIndicesCount)
        {
            //Second buffer - holds the indices (Element Array Buffer - EAB):
            GLuint eab;
            glGenBuffers(1, &eab);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.mIndicesCount * sizeof(GLuint), data.second.data(), GL_STATIC_DRAW);
        }

        auto mesh = std::make_shared<MeshData>(meshData);
        mMeshes[name] = mesh;
        glBindVertexArray(0);

        return mesh;
    }

    return nullptr;
}

std::shared_ptr<MeshData> ResourceManager::getMesh(const std::string& name)
{
    if(mMeshes.find(name) != mMeshes.end())
    {
        return mMeshes[name];
    }
    qDebug() << "No mesh named " << QString::fromStdString(name) << " could be found";
    return nullptr;
}

void ResourceManager::loadWav(const std::string& name, const std::string& path)
{
    auto waveData = new wave_t();
    if (!WavFileHandler::loadWave(path, waveData))
    {
        qDebug() << "Error loading wave file " << QString::fromStdString(name) << "!";
        return;
    }

    std::ostringstream i2s;
    i2s << waveData->dataSize;
    qDebug() << "DataSize: " << QString::fromStdString(i2s.str()) << " bytes";

    if(!OpenALManager::checkOpenALError(alGetError()))
    {
        return;
    }

    mWavFiles[name] = waveData;

    qDebug() << "Loaded " << QString::fromStdString(path);
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

SoundSource* ResourceManager::createSource(const std::string& wav, bool loop, float gain)
{
    if(mWavFiles.find(wav) != mWavFiles.end())
    {
        auto waveData = mWavFiles[wav];

        ALuint frequency{};
        ALenum format{};

        frequency = waveData->sampleRate;

        switch (waveData->bitsPerSample)
        {
        case 8:
            switch (waveData->channels)
            {
            case 1:
                format = AL_FORMAT_MONO8;
                //qDebug() << "Format: 8bit Mono";
                break;
            case 2:
                format = AL_FORMAT_STEREO8;
                //qDebug() << "Format: 8bit Stereo";
                break;
            default: break;
            }
            break;
        case 16:
            switch (waveData->channels)
            {
            case 1:
                format = AL_FORMAT_MONO16;
               // qDebug() << "Format: 16bit Mono";
                break;
            case 2:
                format = AL_FORMAT_STEREO16;
                //qDebug() << "Format: 16bit Stereo";
                break;
            default: break;
            }
            break;
        default: break;
        }

        if (waveData->buffer == nullptr)
        {
            qDebug() << "Error: No wave data!";
        }

        if(!OpenALManager::checkOpenALError(alGetError()))
        {
            return nullptr;
        }

        auto sound = new SoundSource(wav, loop, gain);
        alBufferData(sound->mBuffer, format, waveData->buffer, static_cast<ALsizei>(waveData->dataSize), static_cast<ALsizei>(frequency));
        alSourcei(sound->mSource, AL_BUFFER, static_cast<ALint>(sound->mBuffer));

        if(!OpenALManager::checkOpenALError(alGetError()))
        {
            return nullptr;
        }

        return sound;
    }

    qDebug() << "Could not find " << QString::fromStdString(wav);
    return nullptr;
}

std::pair<std::vector<Vertex>, std::vector<GLuint>> ResourceManager::readObjFile(std::string filename)
{
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;

    //Open File
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + filename;
    std::ifstream fileIn;
    fileIn.open (fileWithPath, std::ifstream::in);
    if(!fileIn)
        qDebug() << "Could not open file for reading: " << QString::fromStdString(filename);

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
                index = std::stoi(segmentArray[0]);     //first is vertex
                if (segmentArray[1] != "")              //second is uv
                    uv = std::stoi(segmentArray[1]);
                else
                {
                    //qDebug() << "No uvs in mesh";       //uv not present
                    uv = 0;                             //this will become -1 in a couple of lines
                }
                normal = std::stoi(segmentArray[2]);    //third is normal

                //Fixing the indexes
                //because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1)    //uv present!
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], tempUVs[uv]);
                    mVertices.push_back(tempVert);
                }
                else            //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], gsl::Vector2D(0.0f, 0.0f));
                    mVertices.push_back(tempVert);
                }
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
    qDebug() << "Obj file read: " << QString::fromStdString(filename);

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
        qDebug() << "TriangleSurface file read: " << QString::fromStdString(filename);
    }
    else
    {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(filename);
    }

    return {mVertices, mIndices};
}

ResourceManager::ResourceManager(){}
