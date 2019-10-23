#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "innpch.h"
#include "meshdata.h"
#include <QtMath> // temp for qDegreesRadians in spot light component

// For ScriptComponent
#include <QFile>
#include <QJSEngine>
#include "scriptsystem.h"
#include "qentity.h"

#include <QJsonObject>

enum class ComponentType
{
    Transform,
    Physics,
    Mesh,
    Camera,
    Input,
    Sound,
    LightPoint,
    LightDirectional,
    LightSpot,
    Script,
    Collider,
    Other
};

// Used by UI so it knows what components are available to add if the entity doesnt have one.
const std::vector<ComponentType> ComponentTypes = {ComponentType::Mesh, ComponentType::Transform, ComponentType::Physics,
                                                   ComponentType::Input, ComponentType::Sound, ComponentType::LightPoint,
                                                   ComponentType::LightDirectional, ComponentType::LightSpot, ComponentType::Script,
                                                   ComponentType::Collider};


struct Component
{
    unsigned int entityId;
    bool valid : 1;
    ComponentType type;

    Component(unsigned int _eID = 0, bool _valid = false, ComponentType typeIn = ComponentType::Other)
        : entityId{_eID}, valid(_valid), type(typeIn)
    {}

    virtual void reset()=0;

    virtual QJsonObject toJSON();
    virtual void fromJSON(QJsonObject object);
};

struct EntityInfo : public Component
{
    std::string name{};

    EntityInfo(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Other)
    {}

    virtual void reset() override
    {
        name = "";
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct TransformComponent : public Component
{
    bool updated : 1;
    std::vector<unsigned int> children;
    gsl::Vector3D position{};
    gsl::Vector3D scale{1,1,1};
    gsl::Quaternion rotation{};
    bool meshBoundsOutdated : 1;
    bool colliderBoundsOutdated : 1;

    TransformComponent(unsigned int _eID = 0, bool _valid = false,
                    const gsl::vec3& _pos = gsl::vec3{},
                    const gsl::vec3& _scale = gsl::vec3{1.f, 1.f, 1.f},
                    const gsl::quat& _rot = gsl::quat{})
        : Component (_eID, _valid, ComponentType::Transform), updated{true}, position{_pos},
          rotation{_rot}, scale{_scale}, meshBoundsOutdated{true}, colliderBoundsOutdated{true}
    {}

    virtual void reset() override
    {
        updated = true;
        children.clear();
        position = gsl::vec3{};
        rotation = gsl::Quaternion{};
        scale = gsl::vec3{1};
        meshBoundsOutdated = true;
        colliderBoundsOutdated = true;
    }

    void addPosition(const gsl::vec3& pos);
    void addRotation(const gsl::quat& rot);
    void addScale(const gsl::vec3& scl);
    void setPosition(const gsl::vec3& pos);
    void setRotation(const gsl::quat& rot);
    void setScale(const gsl::vec3& scl);

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
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

    virtual void reset() override
    {
        velocity = gsl::vec3{};
        acceleration = gsl::vec3{};
        mass = 1.f;
    }
    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct MeshComponent : public Component
{
    bool isVisible : 1;
    MeshData meshData{};
    Material mMaterial{};
    bool renderWireframe{};

    /* World Space Bounds of meshComponent
     * If transform component is missing,
     * bounds will be the same meshdata's
     * local bounds.
     */
    MeshData::Bounds bounds{};

    MeshComponent(unsigned int _eID = 0, bool _valid = false,
                  const MeshData& _meshData = MeshData{}, const Material& _material = Material{}, bool _visible = false)
        : Component (_eID, _valid, ComponentType::Mesh), isVisible{_visible}, meshData{_meshData}, mMaterial{_material}
    {}

    virtual void reset() override
    {
        isVisible = true;
        meshData = MeshData{};
        mMaterial = Material{};
        renderWireframe = false;
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct CameraComponent : public Component
{
    bool isCurrentActive;
    GLuint framebufferTarget;
    float pitch{0.f}, yaw{0.f};
    gsl::Matrix4x4 viewMatrix;
    gsl::Matrix4x4 projectionMatrix;

    CameraComponent(unsigned int _eID = 0, bool _valid = false,
           bool currentActive = true, GLuint fbTarget = 0, const gsl::mat4& vMat = gsl::mat4{},
           const gsl::mat4& pMat = gsl::mat4{})
        : Component (_eID, _valid), isCurrentActive{currentActive},
          framebufferTarget{fbTarget}, viewMatrix{vMat}, projectionMatrix{pMat}
    {}

    virtual void reset() override
    {
        isCurrentActive = true;
        framebufferTarget = 0;
        pitch = yaw = 0.f;
        viewMatrix = gsl::mat4{};
        projectionMatrix = gsl::mat4{};
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct InputComponent : public Component
{
    bool isCurrentlyControlled{false};

    InputComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Input)
    {}

    virtual void reset() override
    {
        isCurrentlyControlled = false;
    }
    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct SoundComponent : public Component
{
    bool isLooping = false;
    bool isMuted = false;
    int mSource;
    float pitch = 1.f;
    float gain = .7f; //JT was here, Savner deg Ole, 3D er bra men vi mangler deg. *Smask*
    std::string name;

    SoundComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Sound), mSource{-1}
    {}

    virtual void reset() override
    {
        isLooping = false;
        isMuted = false;
        mSource = -1;
        pitch = 1.f;
        gain = .7f;
        name = "";
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct PointLightComponent : public Component
{
    gsl::vec3 color;
    float intensity;
    float linear;
    float quadratic;
    float maxBrightness;

    PointLightComponent(unsigned int _eID = 0, bool _valid = false,
                        gsl::vec3 _color = gsl::vec3(1.f, 1.f, 1.f),
                        float _intensity = 1.f,
                        float _linear = 0.045f,
                        float _quadratic = 0.0075f,
                        float _maxBrightness = 2.f)
        : Component(_eID, _valid, ComponentType::LightPoint),
          color(_color), intensity(_intensity),
          linear(_linear), quadratic(_quadratic), maxBrightness(_maxBrightness)
    {}

    virtual void reset() override
    {
        color = gsl::vec3{1};
        intensity = 1.f;
        linear = 0.045f;
        quadratic = 0.0075f;
        maxBrightness = 2.f;
    }

    float calculateRadius() const
    {
        float constant = 1.0f;
        return (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness)))
                / (2.0f * quadratic);
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct SpotLightComponent : public Component
{
    gsl::vec3 color;
    float intensity;
    float cutOff;
    float outerCutOff;
    float linear;
    float quadratic;
    float constant;

    SpotLightComponent(unsigned int _eID = 0, bool _valid = false,
                       gsl::vec3 _color = gsl::vec3(1.f, 1.f, 1.f),
                       float _intensity = 1.f,
                       float _cutOff = qDegreesToRadians(25.f),
                       float _outerCutOff = qDegreesToRadians(35.f),
                       float _linear = 0.045f,
                       float _quadratic = 0.0075f,
                       float _constant = 1.0f)
        : Component(_eID, _valid, ComponentType::LightSpot),
           color(_color), intensity(_intensity),
          cutOff(_cutOff), outerCutOff(_outerCutOff), linear(_linear),
          quadratic(_quadratic), constant(_constant)
    {}

    virtual void reset() override
    {
        color = gsl::vec3{1};
        intensity = 1.f;
        cutOff = qDegreesToRadians(25.f);
        outerCutOff = qDegreesToRadians(35.f);
        linear = 0.045f;
        quadratic = 0.0075f;
        constant = 1.f;
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct DirectionalLightComponent : public Component
{
    gsl::vec3 color;
    float intensity;

    DirectionalLightComponent(unsigned int _eID = 0, bool _valid = false,
                              gsl::vec3 _color = gsl::vec3(1.f, 1.f, 1.f),
                              float _intensity = 1.f)
        : Component(_eID, _valid, ComponentType::LightDirectional),
          color(_color), intensity(_intensity)
    {}

    virtual void reset() override
    {
        color = gsl::vec3{1};
        intensity = 1.f;
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct ScriptComponent : public Component
{
    QJSEngine* engine;
    std::string filePath;
    QEntity* JSEntity{nullptr};

    ScriptComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Script), filePath(""), JSEntity{nullptr}
    {
        engine = new QJSEngine();
        engine->installExtensions(QJSEngine::ConsoleExtension);
        engine->globalObject().setProperty("engine", engine->newQObject(ScriptSystem::get()));
    }

    virtual void reset() override
    {
        delete JSEntity;
        JSEntity = nullptr;
        filePath = "";
    }

    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

struct ColliderComponent : public Component
{
    enum Type : char
    {
        None = 0,
        AABB,
        BOX,
        SPHERE,
        CAPSULE
    };
    static constexpr const char* typeNames[]{"None", "AABB", "Box", "Sphere", "Capsule"};


    Type collisionType;

    ColliderComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Collider), collisionType(AABB)
    {}

    virtual void reset() override
    {
        collisionType = AABB;
    }

    std::variant<gsl::vec3, float, std::pair<float, float>> extents;
    struct Bounds
    {
        gsl::vec3 centre;
        gsl::vec3 extents;

        std::pair<gsl::vec3, gsl::vec3> minMax() const;
    } bounds;


    virtual QJsonObject toJSON() override;
    virtual void fromJSON(QJsonObject object) override;
};

// .. etc

#endif // COMPONENTS_H
