#include "componentdata.h"
#include <stack>
#include <QJsonArray>
#include "resourcemanager.h"
#include "soundmanager.h"
#include "scriptsystem.h"
#include "qentity.h"
#include <QFileInfo>
#include "world.h"

void TransformComponent::addPosition(const gsl::vec3 &pos)
{
    position += pos;
    updated = true;
}

void TransformComponent::addRotation(const gsl::quat &rot)
{
    rotation *= rot;
    updated = true;
    colliderBoundsOutdated = true;
}

void TransformComponent::addScale(const gsl::vec3 &scl)
{
    scale += scl;
    updated = true;
    meshBoundsOutdated = true;
    colliderBoundsOutdated = true;
}

void TransformComponent::setPosition(const gsl::vec3 &pos)
{
    position = pos;
    updated = true;
}

void TransformComponent::setRotation(const gsl::quat &rot)
{
    rotation = rot;
    updated = true;
    colliderBoundsOutdated = true;
}

void TransformComponent::setScale(const gsl::vec3 &scl)
{
    scale = scl;
    updated = true;
    meshBoundsOutdated = true;
    colliderBoundsOutdated = true;
}

QJsonObject TransformComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    QJsonArray childs;
    for(unsigned i = 0; i < children.size(); ++i)
    {
        childs.insert(i, QJsonValue(static_cast<int>(children[i])));
    }
    parentObj.insert("Children", childs);

    QJsonArray positionArr, rotationArr, scaleArr;

    positionArr.insert(0, QJsonValue(static_cast<double>(position.x)));
    positionArr.insert(1, QJsonValue(static_cast<double>(position.y)));
    positionArr.insert(2, QJsonValue(static_cast<double>(position.z)));
    parentObj.insert("Position", positionArr);

    rotationArr.insert(0, QJsonValue(static_cast<double>(rotation.s)));
    rotationArr.insert(1, QJsonValue(static_cast<double>(rotation.i)));
    rotationArr.insert(2, QJsonValue(static_cast<double>(rotation.j)));
    rotationArr.insert(3, QJsonValue(static_cast<double>(rotation.k)));
    parentObj.insert("Rotation", rotationArr);

    scaleArr.insert(0, QJsonValue(static_cast<double>(scale.x)));
    scaleArr.insert(1, QJsonValue(static_cast<double>(scale.y)));
    scaleArr.insert(2, QJsonValue(static_cast<double>(scale.z)));
    parentObj.insert("Scale", scaleArr);

    parentObj.insert("ColliderBoundsOutdated", colliderBoundsOutdated);

    return parentObj;
}

void TransformComponent::fromJSON(QJsonObject object)
{
    auto childsArray = object["Children"].toArray();
    for(auto child : childsArray)
    {
        auto baby = static_cast<unsigned>(child.toInt());
        if (std::find(children.begin(), children.end(), baby) == children.end())
            children.push_back(baby);
    }

    auto& world = World::getWorld();

    auto positionArray = object["Position"].toArray();
    world.getEntityManager()->setTransformPos(entityId, {
                                                  static_cast<float>(positionArray[0].toDouble()),
                                                  static_cast<float>(positionArray[1].toDouble()),
                                                  static_cast<float>(positionArray[2].toDouble())
                                              });

    auto rotationArray = object["Rotation"].toArray();
    world.getEntityManager()->setTransformRot(entityId, {
                                                  static_cast<float>(rotationArray[0].toDouble()),
                                                  static_cast<float>(rotationArray[1].toDouble()),
                                                  static_cast<float>(rotationArray[2].toDouble()),
                                                  static_cast<float>(rotationArray[3].toDouble())
                                              });



    auto scaleArray = object["Scale"].toArray();
    world.getEntityManager()->setTransformScale(entityId, {
                                                    static_cast<float>(scaleArray[0].toDouble()),
                                                    static_cast<float>(scaleArray[1].toDouble()),
                                                    static_cast<float>(scaleArray[2].toDouble())
                                                });

    colliderBoundsOutdated = object["ColliderBoundsOutdated"].toBool(true);
}

ScriptComponent::ScriptComponent(unsigned int _eID, bool _valid)
    : Component(_eID, _valid, ComponentType::Script),
    filePath(""), JSEntity{nullptr}, beginplayRun{false}
{
    engine = new QJSEngine();
    engine->installExtensions(QJSEngine::ConsoleExtension);
    engine->globalObject().setProperty("engine", engine->newQObject(new QScriptSystemPointer{}));
}

ScriptComponent::ScriptComponent(ScriptComponent &&rhs)
    : Component{rhs}, engine{rhs.engine}, filePath{std::move(rhs.filePath)}, JSEntity{rhs.JSEntity}, beginplayRun{rhs.beginplayRun}
{
    rhs.engine = nullptr;
    rhs.JSEntity = nullptr;
}

ScriptComponent &ScriptComponent::operator=(ScriptComponent &&rhs)
{
    entityId = rhs.entityId;
    valid = rhs.valid;

    engine = rhs.engine;
    rhs.engine = nullptr;
    JSEntity = rhs.JSEntity;
    rhs.JSEntity = nullptr;

    filePath = std::move(rhs.filePath);
    beginplayRun = rhs.beginplayRun;

    return *this;
}

void ScriptComponent::reset()
{
    delete JSEntity;
    JSEntity = nullptr;
    delete engine;
    engine = nullptr;

    filePath = "";
    beginplayRun = false;

    engine = new QJSEngine();
    engine->installExtensions(QJSEngine::ConsoleExtension);
    engine->globalObject().setProperty("engine", engine->newQObject(new QScriptSystemPointer{}));
}

ScriptComponent::~ScriptComponent()
{
    delete JSEntity;
    delete engine;
}

QJsonObject ScriptComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("FilePath", QJsonValue(filePath.c_str()));

    return parentObj;
}

void ScriptComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    auto file = object["FilePath"].toString();
    if(file.size())
    {
        engine->globalObject().setProperty("entityID", entityId);
        ScriptSystem::get()->load(*this, file.toStdString());
    }
}

std::pair<gsl::vec3, gsl::vec3> ColliderComponent::Bounds::minMax() const {
    return {centre - extents * 0.5f, centre + extents * 0.5f};
}

QJsonObject Component::toJSON()
{
    QJsonObject returnObject;
    returnObject.insert("ComponentType", QJsonValue(static_cast<int>(type)));
    return returnObject;
}

void Component::fromJSON(QJsonObject object)
{

}

QJsonObject EntityInfo::toJSON()
{
    auto parentObj = Component::toJSON();
    parentObj.insert("Name", QJsonValue(name.c_str()));
    parentObj.insert("ShouldShowInEditor", QJsonValue(shouldShowInEditor));
    return parentObj;
}

void EntityInfo::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    name = object["Name"].toString().toStdString();
    shouldShowInEditor = object["ShouldShowInEditor"].toBool();
}

void PhysicsComponent::setVelocity(gsl::vec3 newVel)
{
    velocity.x = gsl::equal(velocity.x, newVel.x) ? velocity.x : newVel.x;
    velocity.y = gsl::equal(velocity.y, newVel.y) ? velocity.y : newVel.y;
    velocity.z = gsl::equal(velocity.z, newVel.z) ? velocity.z : newVel.z;
}

void PhysicsComponent::setAcceleration(gsl::vec3 newAcc)
{
    acceleration.x = gsl::equal(acceleration.x, newAcc.x) ? acceleration.x : newAcc.x;
    acceleration.y = gsl::equal(acceleration.y, newAcc.y) ? acceleration.y : newAcc.y;
    acceleration.z = gsl::equal(acceleration.z, newAcc.z) ? acceleration.z : newAcc.z;
}

QJsonObject PhysicsComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    QJsonArray velocityArr, accelerationArr;

    velocityArr.insert(0, QJsonValue(static_cast<double>(velocity.x)));
    velocityArr.insert(1, QJsonValue(static_cast<double>(velocity.y)));
    velocityArr.insert(2, QJsonValue(static_cast<double>(velocity.z)));
    parentObj.insert("Velocity", velocityArr);

    accelerationArr.insert(0, QJsonValue(static_cast<double>(acceleration.x)));
    accelerationArr.insert(1, QJsonValue(static_cast<double>(acceleration.y)));
    accelerationArr.insert(2, QJsonValue(static_cast<double>(acceleration.z)));
    parentObj.insert("Acceleration", accelerationArr);

    parentObj.insert("Mass", QJsonValue(static_cast<double>(mass)));

    return parentObj;
}

void PhysicsComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    auto velocityArr = object["Velocity"].toArray();
    setVelocity({static_cast<float>(velocityArr[0].toDouble()),
                 static_cast<float>(velocityArr[1].toDouble()),
                 static_cast<float>(velocityArr[2].toDouble())});

    auto accelerationArr = object["Acceleration"].toArray();
    setAcceleration({static_cast<float>(velocityArr[0].toDouble()),
                     static_cast<float>(velocityArr[1].toDouble()),
                     static_cast<float>(velocityArr[2].toDouble())});

    mass = static_cast<float>(object["Mass"].toDouble());
}

QJsonObject MeshComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("IsVisible", QJsonValue(isVisible));
    parentObj.insert("MeshData", meshData.toJSON());
    parentObj.insert("MaterialData", mMaterial.toJSON());
    parentObj.insert("RenderWireframe", QJsonValue(renderWireframe));

    return parentObj;
}

void MeshComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    isVisible = object["IsVisible"].toBool();

    // Mesh data

    auto meshDataObj = object["MeshData"].toObject();

    auto meshName = meshDataObj["Name"].toString().toStdString();
    if(meshName.size() && meshName != "None")
    {
        meshData = *ResourceManager::instance().getMesh(meshName);
        meshData.mRenderType = static_cast<GLenum>(meshDataObj["RenderType"].toInt(GL_TRIANGLES));
    }

    // Material data

    auto materialObj = object["MaterialData"].toObject();
    auto shaderName = materialObj["Shader"].toString();
    if(shaderName.size() && shaderName != "None")
    {
        mMaterial.loadShaderWithParameters(ResourceManager::instance().getShader(shaderName.toStdString()));
    }

    auto parametersArray = materialObj["Parameters"].toArray();
    if(parametersArray.size())
    {
        for(auto parameterRef : parametersArray)
        {
            if(parameterRef.isObject())
            {
                auto parameterObj = parameterRef.toObject();

                for(auto it = parameterObj.begin(); it != parameterObj.end(); ++it)
                {
                    auto name = it.key().toStdString();
                    if(mMaterial.mParameters.find(name) != mMaterial.mParameters.end())
                    {
                        if(it.value().isDouble())
                        {
                            mMaterial.mParameters[name] = static_cast<float>(it.value().toDouble());
                        }
                        else if(it.value().isArray())
                        {
                            auto array = it.value().toArray();
                            if(array.size() == 2)
                            {
                                gsl::vec2 vector;
                                vector.x = static_cast<float>(array[0].toDouble());
                                vector.y = static_cast<float>(array[1].toDouble());
                                mMaterial.mParameters[name] = vector;
                            }
                            else if(array.size() == 3)
                            {
                                gsl::vec3 vector;
                                vector.x = static_cast<float>(array[0].toDouble());
                                vector.y = static_cast<float>(array[1].toDouble());
                                vector.z = static_cast<float>(array[2].toDouble());
                                mMaterial.mParameters[name] = vector;
                            }
                            else if(array.size() == 4)
                            {
                                gsl::vec4 vector;
                                vector.x = static_cast<float>(array[0].toDouble());
                                vector.y = static_cast<float>(array[1].toDouble());
                                vector.z = static_cast<float>(array[2].toDouble());
                                vector.w = static_cast<float>(array[2].toDouble());
                                mMaterial.mParameters[name] = vector;
                            }
                        }
                        else
                        {
                            // For now just choose int if it wasn't one of the others (float, vec2, vec3 or vec4).
                            mMaterial.mParameters[name] = it.value().toInt();
                        }
                    }
                }
            }
        }
    }

    for(auto textureRef : object["Textures"].toArray())
    {
        auto textureObj = textureRef.toObject();
        mMaterial.mTextures.push_back(
                    std::make_pair(
                        static_cast<unsigned>(textureObj["ID"].toInt()),
                        static_cast<GLenum>(textureObj["Type"].toInt()))
                );
    }


    renderWireframe = object["RenderWireframe"].toBool();
}

QJsonObject CameraComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("EditorCam", QJsonValue(isEditorCamera));
    parentObj.insert("Pitch", QJsonValue(static_cast<double>(pitch)));
    parentObj.insert("Yaw", QJsonValue(static_cast<double>(yaw)));

    return parentObj;
}

void CameraComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    isEditorCamera = object["EditorCam"].toBool();
    pitch = static_cast<float>(object["Pitch"].toDouble());
    yaw = static_cast<float>(object["Yaw"].toDouble());
}

QJsonObject InputComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("ControlledWhilePlaying", QJsonValue(controlledWhilePlaying));

    return parentObj;
}

void InputComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    controlledWhilePlaying = object["ControlledWhilePlaying"].toBool();
}

QJsonObject SoundComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("Looping", QJsonValue(isLooping));
    parentObj.insert("Muted", QJsonValue(isMuted));
    parentObj.insert("Name", QJsonValue(name.c_str()));
    parentObj.insert("Pitch", QJsonValue(static_cast<double>(pitch)));
    parentObj.insert("Gain", QJsonValue(static_cast<double>(gain)));

    return parentObj;
}

void SoundComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    isLooping = object["Looping"].toBool();
    isMuted = object["Muted"].toBool();
    name = object["Name"].toString().toStdString();
    pitch = static_cast<float>(object["Pitch"].toDouble());
    gain = static_cast<float>(object["Gain"].toDouble());

    if(name.size())
        SoundManager::get().createSource(this, name);
}

QJsonObject PointLightComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    QJsonArray colorArr;
    colorArr.insert(0, QJsonValue(static_cast<double>(color.x)));
    colorArr.insert(1, QJsonValue(static_cast<double>(color.y)));
    colorArr.insert(2, QJsonValue(static_cast<double>(color.z)));
    parentObj.insert("Color", colorArr);

    parentObj.insert("Intensity", static_cast<double>(intensity));
    parentObj.insert("Linear", static_cast<double>(linear));
    parentObj.insert("Quadratic", static_cast<double>(quadratic));
    parentObj.insert("MaxBrightness", static_cast<double>(maxBrightness));

    return parentObj;
}

void PointLightComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    auto colorArray = object["Color"].toArray();
    color.x = static_cast<float>(colorArray[0].toDouble());
    color.y = static_cast<float>(colorArray[1].toDouble());
    color.z = static_cast<float>(colorArray[2].toDouble());

    intensity = static_cast<float>(object["Intensity"].toDouble());
    linear = static_cast<float>(object["Linear"].toDouble());
    quadratic = static_cast<float>(object["Quadratic"].toDouble());
    maxBrightness = static_cast<float>(object["MaxBrightness"].toDouble());
}

QJsonObject SpotLightComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    QJsonArray colorArr;
    colorArr.insert(0, QJsonValue(static_cast<double>(color.x)));
    colorArr.insert(1, QJsonValue(static_cast<double>(color.y)));
    colorArr.insert(2, QJsonValue(static_cast<double>(color.z)));
    parentObj.insert("Color", colorArr);

    parentObj.insert("Intensity", static_cast<double>(intensity));
    parentObj.insert("CutOff", static_cast<double>(cutOff));
    parentObj.insert("OuterCutOff", static_cast<double>(outerCutOff));
    parentObj.insert("Linear", static_cast<double>(linear));
    parentObj.insert("Quadratic", static_cast<double>(quadratic));
    parentObj.insert("Constant", static_cast<double>(constant));

    return parentObj;
}

void SpotLightComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    auto colorArray = object["Color"].toArray();
    color.x = static_cast<float>(colorArray[0].toDouble());
    color.y = static_cast<float>(colorArray[1].toDouble());
    color.z = static_cast<float>(colorArray[2].toDouble());

    intensity = static_cast<float>(object["Intensity"].toDouble());
    cutOff = static_cast<float>(object["CutOff"].toDouble());
    outerCutOff = static_cast<float>(object["OuterCutOff"].toDouble());
    linear = static_cast<float>(object["Linear"].toDouble());
    quadratic = static_cast<float>(object["Quadratic"].toDouble());
    constant = static_cast<float>(object["Constant"].toDouble());
}

QJsonObject DirectionalLightComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    QJsonArray colorArr;
    colorArr.insert(0, QJsonValue(static_cast<double>(color.x)));
    colorArr.insert(1, QJsonValue(static_cast<double>(color.y)));
    colorArr.insert(2, QJsonValue(static_cast<double>(color.z)));
    parentObj.insert("Color", colorArr);

    parentObj.insert("Intensity", static_cast<double>(intensity));

    return parentObj;
}

void DirectionalLightComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    auto colorArray = object["Color"].toArray();
    color.x = static_cast<float>(colorArray[0].toDouble());
    color.y = static_cast<float>(colorArray[1].toDouble());
    color.z = static_cast<float>(colorArray[2].toDouble());

    intensity = static_cast<float>(object["Intensity"].toDouble());
}

QJsonObject ColliderComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("CollisionType", static_cast<int>(collisionType));
    switch (collisionType)
    {
        case ColliderComponent::AABB:
        case ColliderComponent::BOX:
        {
            auto ext = std::get_if<gsl::vec3>(&extents);
            QJsonArray arr;
            arr.insert(0, (ext != nullptr) ? static_cast<double>(ext->x) : 1.0);
            arr.insert(1, (ext != nullptr) ? static_cast<double>(ext->y) : 1.0);
            arr.insert(2, (ext != nullptr) ? static_cast<double>(ext->z) : 1.0);
            parentObj.insert("Extents", arr);
        }
        break;
        case ColliderComponent::SPHERE:
        {
            auto ext = std::get_if<float>(&extents);
            parentObj.insert("Extents", static_cast<double>((ext != nullptr) ? *ext : 1.f));
        }
        break;
        case ColliderComponent::CAPSULE:
        {
            auto ext = std::get_if<std::pair<float, float>>(&extents);
            QJsonObject obj;
            obj.insert("Radius", (ext != nullptr) ? static_cast<double>(ext->first) : 1.0);
            obj.insert("Half-height", (ext != nullptr) ? static_cast<double>(ext->second) : 1.0);
            parentObj.insert("Extents", obj);
        }
        break;
        default:
        break;
    }

    return parentObj;
}

void ColliderComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    collisionType = static_cast<ColliderComponent::Type>(object["CollisionType"].toInt(0));

    auto obj = object["Extents"];
    switch (collisionType)
    {
        case ColliderComponent::AABB:
        case ColliderComponent::BOX:
        {
            gsl::vec3 ext{1.f, 1.f, 1.f};
            if (obj.isArray())
            {
                auto arr = obj.toArray();
                for (int i{0}; i < 3 && i < arr.size(); ++i)
                    ext[i] = static_cast<float>(arr[i].toDouble(1.0));
            }

            extents = ext;
        }
        break;
        case ColliderComponent::SPHERE:
            extents = static_cast<float>(obj.toDouble(1.0));
        break;
        case ColliderComponent::CAPSULE:
        {
            std::pair<float, float> ext{1.f, 1.f};
            if (obj.isObject())
            {
                ext.first = static_cast<float>(obj.toObject()["Radius"].toDouble(1.0));
                ext.second = static_cast<float>(obj.toObject()["Half-height"].toDouble(1.0));
            }
            extents = ext;
        }
        break;
        default:
            break;
    }
}

void ParticleComponent::reset()
{

}

QJsonObject ParticleComponent::toJSON()
{
    return {};
}

void ParticleComponent::fromJSON(QJsonObject object)
{

}
