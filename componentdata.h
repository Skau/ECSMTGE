#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "innpch.h"
#include "meshdata.h"
#include <QtMath> // temp for qDegreesRadians in spot light component

// For ScriptComponent
#include <QFile>
#include <QJSEngine>
#include "scriptsystem.h"

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
    Other
};

// Used by UI so it knows what components are available to add if the entity doesnt have one.
const std::vector<ComponentType> ComponentTypes = {ComponentType::Mesh, ComponentType::Transform, ComponentType::Physics,
                                                   ComponentType::Input, ComponentType::Sound, ComponentType::LightPoint,
                                                   ComponentType::LightDirectional, ComponentType::LightSpot, ComponentType::Script};


struct Component
{
    unsigned int entityId;
    bool valid : 1;
    ComponentType type;

    Component(unsigned int _eID = 0, bool _valid = false, ComponentType typeIn = ComponentType::Other)
        : entityId{_eID}, valid(_valid), type(typeIn)
    {}
};

struct EntityInfo : public Component
{
    std::string name{};

    EntityInfo(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Other)
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
    void addScale(const gsl::vec3& scl);
    void setPosition(const gsl::vec3& pos);
    void setRotation(const gsl::quat& rot);
    void setScale(const gsl::vec3& scl);
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
    bool isVisible : 1;
    MeshData meshData{};

    MeshComponent(unsigned int _eID = 0, bool _valid = false,
           const MeshData& _meshData = MeshData{}, bool _visible = false)
        : Component (_eID, _valid, ComponentType::Mesh), isVisible{_visible}, meshData{_meshData}
    {}
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
    bool isLooping = false;
    bool isMuted = false;
    int mSource;
    float pitch = 1.f;
    float gain = .7f;
    std::string name;

    SoundComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Sound), mSource{-1}
    {}
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

    float calculateRadius() const
    {
        float constant = 1.0f;
        return (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness)))
                / (2.0f * quadratic);
    }
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
};

struct ScriptComponent : public Component
{
private:
    QJSEngine* engine;
    std::string filePath;

public:
    ScriptComponent(unsigned int _eID = 0, bool _valid = false)
        : Component(_eID, _valid, ComponentType::Script), filePath("")
    {
        engine = new QJSEngine();
        engine->installExtensions(QJSEngine::ConsoleExtension);
        engine->globalObject().setProperty("engine", engine->newQObject(ScriptSystem::get()));
        engine->globalObject().setProperty("entityID", _eID);
    }

    const std::string& getFilePath() { return filePath; }

    /**
     * @brief Loads the js file given. Returns true if the file is successfully evaluated and set.
     */
    bool load(const std::string& file);

    /**
     * @brief Calls a JS function on the current file loaded. Returns true if successful.
     */
    bool call(const std::string& function);

    /**
     * @brief Calls a JS function with params on the current file loaded. Returns true if successful.
     */
    bool call(const std::string& function, QJSValueList params);

    /**
     * @brief Executes one off raw js code from the mini editor. Returns true if successfull.
     */
    bool execute(QString function, QString contents, QString fileName);
};

// .. etc

#endif // COMPONENTS_H
