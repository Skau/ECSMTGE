#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "innpch.h"
#include "meshdata.h"

// Something like this?

struct Component
{
    unsigned int entityId;
    bool valid : 1;


    Component(unsigned int _eID = 0, bool _valid = false)
        : entityId{_eID}, valid(_valid)
    {}
};

struct EntityData : public Component
{
    std::string name{};
};

struct Transform : public Component
{
    bool updated{true};
    gsl::Vector3D position{};
    gsl::Vector3D scale{1,1,1};
    gsl::Vector3D rotation{};


    Transform(unsigned int _eID = 0, bool _valid = false,
              const gsl::vec3& _pos = gsl::vec3{},
              const gsl::vec3& _scale = gsl::vec3{1.f, 1.f, 1.f},
              const gsl::vec3& _rot = gsl::vec3{})
        : Component (_eID, _valid), updated{true}, position{_pos},
          scale{_scale}, rotation{_rot}
    {}
};

struct Render : public Component
{
    MeshData meshData{};
    bool isVisible : 1;


    Render(unsigned int _eID = 0, bool _valid = false,
           const MeshData& _meshData = MeshData{}, bool _visible = false)
        : Component (_eID, _valid), meshData{_meshData}, isVisible{_visible}
    {}
};

struct Camera : public Component
{
    GLuint framebufferTarget;
    gsl::Matrix4x4 viewMatrix;
    gsl::Matrix4x4 projectionMatrix;


    Camera(unsigned int _eID = 0, bool _valid = false,
           GLuint fbTarget = 0, const gsl::mat4& vMat = gsl::mat4{},
           const gsl::mat4& pMat = gsl::mat4{})
        : Component (_eID, _valid), framebufferTarget{fbTarget},
          viewMatrix{vMat}, projectionMatrix{pMat}
    {}
};

// .. etc

#endif // COMPONENTS_H
