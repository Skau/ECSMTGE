#ifndef MESHDATA_H
#define MESHDATA_H

#include <memory>
#include "innpch.h"
#include "gltypes.h"
#include "texture.h"


struct Material
{
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    int mTexture{-1};
    std::shared_ptr<Shader> mShader{nullptr};

    Material(const gsl::Vector3D& objectColor = {1.f, 1.f, 1.f}, int texture = -1, std::shared_ptr<Shader> shader = std::shared_ptr<Shader>())
        : mObjectColor(objectColor), mTexture(texture), mShader(shader) {}

};

struct MeshData
{
    GLenum mRenderType{};
    std::array<unsigned, 3> mVAOs{};
    unsigned int mVAO{};
    std::array<unsigned, 3> mVerticesCounts{};
    unsigned int mVerticesCount{};
    std::array<unsigned, 3> mIndicesCounts{};
    unsigned int mIndicesCount{};
    std::string mName{};
    Material mMaterial;

    MeshData(unsigned int VAO = 0, unsigned int verticesCount = 0, unsigned int indicesCount = 0, const std::string& name = "", Material material = Material(), GLenum renderType = GL_TRIANGLES)
        : mRenderType(renderType), mVAO(VAO), mVerticesCount(verticesCount), mIndicesCount(indicesCount), mName(name), mMaterial(material) {}
};

#endif // MESHDATA_H
