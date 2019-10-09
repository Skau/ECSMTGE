#ifndef MESHDATA_H
#define MESHDATA_H

#include <memory>
#include "innpch.h"
#include "gltypes.h"
#include "texture.h"
#include <variant>

typedef std::variant<int, float, gsl::vec2, gsl::vec3, gsl::vec4> ShaderParamType;

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
};

struct MeshData
{
    GLenum mRenderType{};
    std::array<unsigned, 3> mVAOs{};
    std::array<unsigned, 3> mVerticesCounts{};
    std::array<unsigned, 3> mIndicesCounts{};
    std::string mName{};

    MeshData(const std::string& name = "", GLenum renderType = GL_TRIANGLES)
        : mRenderType(renderType), mName(name) {}
};

#endif // MESHDATA_H
