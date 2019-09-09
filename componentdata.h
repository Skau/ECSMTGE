#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "innpch.h"
#include "meshdata.h"

// Something like this?


enum class ComponentType
{
    Transform,
    Mesh,
    Input,
    Other
};

// Used by UI so it knows what components are available to add if the entity doesnt have one.
const std::vector<ComponentType> ComponentTypes = {ComponentType::Mesh, ComponentType::Transform, ComponentType::Input};


struct Component
{
    unsigned int entityId;
    bool valid : 1;
    ComponentType type;

    Component(unsigned int _eID = 0, bool _valid = false, ComponentType typeIn = ComponentType::Other)
        : entityId{_eID}, valid(_valid), type(typeIn)
    {}
};

struct EntityData : public Component
{
    std::string name{};
};

struct TransformComponent : public Component
{
    bool updated{true};
    gsl::Vector3D position{};
    gsl::Vector3D scale{1,1,1};
    gsl::Vector3D rotation{};


    TransformComponent(unsigned int _eID = 0, bool _valid = false,
              const gsl::vec3& _pos = gsl::vec3{},
              const gsl::vec3& _scale = gsl::vec3{1.f, 1.f, 1.f},
              const gsl::vec3& _rot = gsl::vec3{})
        : Component (_eID, _valid, ComponentType::Transform), updated{true}, position{_pos},
          scale{_scale}, rotation{_rot}
    {}
};

struct MeshComponent : public Component
{
    MeshData meshData{};
    bool isVisible : 1;


    MeshComponent(unsigned int _eID = 0, bool _valid = false,
           const MeshData& _meshData = MeshData{}, bool _visible = false)
        : Component (_eID, _valid, ComponentType::Mesh), meshData{_meshData}, isVisible{_visible}
    {}
};

struct CameraComponent : public Component
{
    GLuint framebufferTarget;
    gsl::Matrix4x4 viewMatrix;
    gsl::Matrix4x4 projectionMatrix;


    CameraComponent(unsigned int _eID = 0, bool _valid = false,
           GLuint fbTarget = 0, const gsl::mat4& vMat = gsl::mat4{},
           const gsl::mat4& pMat = gsl::mat4{})
        : Component (_eID, _valid), framebufferTarget{fbTarget},
          viewMatrix{vMat}, projectionMatrix{pMat}
    {}
};

struct InputComponent : public Component
{
    bool isCurrentlyControlled{false};

    InputComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Input)
    {}
};

// .. etc

#endif // COMPONENTS_H
