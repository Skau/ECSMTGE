#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "innpch.h"
#include "meshdata.h"
#include "soundsource.h"

// Something like this?


enum class ComponentType
{
    Transform,
    Physics,
    Mesh,
    Camera,
    Input,
    Other
};

// Used by UI so it knows what components are available to add if the entity doesnt have one.
const std::vector<ComponentType> ComponentTypes = {ComponentType::Mesh, ComponentType::Transform, ComponentType::Physics, ComponentType::Input};


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


    EntityData()
        : Component(0, false, ComponentType::Other)
    {}
};

struct TransformComponent : public Component
{
    bool updated{true};
    std::vector<unsigned int> children;
    gsl::Vector3D position{};
    gsl::Quaternion rotation{};
    gsl::Vector3D scale{1,1,1};


    TransformComponent(unsigned int _eID = 0, bool _valid = false,
                    const gsl::vec3& _pos = gsl::vec3{},
                    const gsl::vec3& _scale = gsl::vec3{1.f, 1.f, 1.f},
                    const gsl::quat& _rot = gsl::quat{})
        : Component (_eID, _valid, ComponentType::Transform), updated{true}, position{_pos},
          rotation{_rot}, scale{_scale}
    {}


    void addPosition(const gsl::vec3& pos);
    void addRotation(const gsl::quat& rot);
    void addScale(const gsl::vec3& scale);
    void setPosition(const gsl::vec3& pos);
    void setRotation(const gsl::quat& rot);
    void setScale(const gsl::vec3& scale);
};


struct PhysicsComponent : public Component
{
    gsl::vec3 velocity{};
    gsl::vec3 acceleration{};
    float mass{1.f};


    PhysicsComponent(unsigned int _eID = 0, bool _valid = false,
                     const gsl::vec3& _velocity = gsl::vec3{})
        : Component (_eID, _valid, ComponentType::Physics), velocity{_velocity}
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
    bool isCurrentActive;
    GLuint framebufferTarget;
    gsl::Matrix4x4 viewMatrix;
    gsl::Matrix4x4 projectionMatrix;


    CameraComponent(unsigned int _eID = 0, bool _valid = false,
           bool currentActive = true, GLuint fbTarget = 0, const gsl::mat4& vMat = gsl::mat4{},
           const gsl::mat4& pMat = gsl::mat4{})
        : Component (_eID, _valid), isCurrentActive{currentActive},
          framebufferTarget{fbTarget}, viewMatrix{vMat}, projectionMatrix{pMat}
    {}
};

struct InputComponent : public Component
{
    bool isCurrentlyControlled{false};

    InputComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Input)
    {}
};

struct SoundComponent : public Component
{
    SoundSource sound;
};

// .. etc

#endif // COMPONENTS_H
