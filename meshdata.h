#ifndef MESHDATA_H
#define MESHDATA_H

#include <memory>
#include "innpch.h"
#include "gltypes.h"
#include "texture.h"
#include <variant>

#include <QJsonObject>
#include <QJsonArray>

struct Material
{
    std::shared_ptr<Shader> mShader{nullptr};
    std::map<std::string, ShaderParamType> mParameters;
    std::vector<std::pair<uint, GLenum>> mTextures;

    Material(std::shared_ptr<Shader> shader = std::shared_ptr<Shader>{},
             std::map<std::string, ShaderParamType> parameters = std::map<std::string, ShaderParamType>{},
             std::vector<std::pair<uint, GLenum>> textures = std::vector<std::pair<uint, GLenum>>{})
        : mShader{shader}, mParameters{parameters}, mTextures{textures}
    {}
    Material(const Material& material) = default;

    void loadShaderWithParameters(std::shared_ptr<Shader> shader)
    {
        mParameters.clear();

        if(shader)
        {
            mShader = shader;
            mParameters = mShader->params;
        }
        else
        {
            mShader = nullptr;
        }
    }

    QJsonObject toJSON()
    {
        QJsonObject returnObject;


        returnObject.insert("Shader", QJsonValue(mShader ? mShader->mName.c_str() : "None"));

        QJsonArray parameterArray;
        int i = 0;
        for(auto& value : mParameters)
        {
            QJsonObject obj;
            if(std::holds_alternative<bool>(value.second))
            {
                obj.insert(value.first.c_str(), QJsonValue(std::get<bool>(value.second)));
            }
            else if (std::holds_alternative<int>(value.second))
            {
                obj.insert(value.first.c_str(), QJsonValue(std::get<int>(value.second)));
            }
            else if (std::holds_alternative<float>(value.second))
            {
                obj.insert(value.first.c_str(), QJsonValue(static_cast<double>(std::get<float>(value.second))));
            }
            else if (std::holds_alternative<gsl::vec2>(value.second))
            {
                QJsonArray array;
                array.insert(0, QJsonValue(static_cast<double>(std::get<gsl::vec2>(value.second).x)));
                array.insert(1, QJsonValue(static_cast<double>(std::get<gsl::vec2>(value.second).y)));
                obj.insert(value.first.c_str(), array);
            }
            else if (std::holds_alternative<gsl::vec3>(value.second))
            {
                QJsonArray array;
                array.insert(0, QJsonValue(static_cast<double>(std::get<gsl::vec3>(value.second).x)));
                array.insert(1, QJsonValue(static_cast<double>(std::get<gsl::vec3>(value.second).y)));
                array.insert(2, QJsonValue(static_cast<double>(std::get<gsl::vec3>(value.second).z)));
                obj.insert(value.first.c_str(), array);
            }
            else if(std::holds_alternative<gsl::vec4>(value.second))
            {
                QJsonArray array;
                array.insert(0, QJsonValue(static_cast<double>(std::get<gsl::vec4>(value.second).x)));
                array.insert(1, QJsonValue(static_cast<double>(std::get<gsl::vec4>(value.second).y)));
                array.insert(2, QJsonValue(static_cast<double>(std::get<gsl::vec4>(value.second).z)));
                array.insert(3, QJsonValue(static_cast<double>(std::get<gsl::vec4>(value.second).w)));
                obj.insert(value.first.c_str(), array);
            }

            parameterArray.insert(i, obj);
            i++;
        }

        returnObject.insert("Parameters", parameterArray);

        QJsonArray textureArray;
        i = 0;
        for(auto& texture : mTextures)
        {
            QJsonObject obj;
            obj.insert("ID", QJsonValue(static_cast<int>(texture.first)));
            obj.insert("Type", QJsonValue(static_cast<int>(texture.second)));
            textureArray.insert(i, obj);
            i++;
        }

        returnObject.insert("Textures", textureArray);

        return returnObject;
    }

    Material& operator= (const Material& other) = default;
    Material& operator= (Material&& other)
    {
        mShader = std::move(other.mShader);
        mParameters = std::move(other.mParameters);
        mTextures = std::move(other.mTextures);
        return *this;
    }
};

struct MeshData
{
    GLenum mRenderType{};
    std::array<unsigned, 3> mVAOs{};
    std::array<unsigned, 3> mVerticesCounts{};
    std::array<unsigned, 3> mIndicesCounts{};
    std::string mName{};
    // The bounds for the mesh in local space
    struct Bounds
    {
        gsl::vec3 centre;
        float radius;
    } bounds;

    MeshData(const std::string& name = "", GLenum renderType = GL_TRIANGLES)
        : mRenderType(renderType), mName(name) {}

    QJsonObject toJSON()
    {
        QJsonObject returnObject;

        returnObject.insert("RenderType", QJsonValue(static_cast<int>(mRenderType)));
        returnObject.insert("Name", QJsonValue(mName.size() ? mName.c_str() : "None"));

        // Not sure if we need bounds in JSON
//        QJsonObject boundsObj;
//        QJsonArray centreArr;
//        centreArr.insert(0, QJsonValue(static_cast<double>(bounds.centre.x)));
//        centreArr.insert(1, QJsonValue(static_cast<double>(bounds.centre.y)));
//        centreArr.insert(2, QJsonValue(static_cast<double>(bounds.centre.z)));
//        boundsObj.insert("Centre", centreArr);
//        boundsObj.insert("Radius", QJsonValue(static_cast<double>(bounds.radius)));
//        returnObject.insert("Bounds", boundsObj);

        return returnObject;
    }
};

#endif // MESHDATA_H
