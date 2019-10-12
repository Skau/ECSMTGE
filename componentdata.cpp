#include "componentdata.h"
#include <stack>
#include <QJsonArray>
#include "resourcemanager.h"
#include "soundmanager.h"

void TransformComponent::addPosition(const gsl::vec3 &pos)
{
    position += pos;
    updated = true;
}

void TransformComponent::addRotation(const gsl::quat &rot)
{
    rotation *= rot;
    updated = true;
}

void TransformComponent::addScale(const gsl::vec3 &scl)
{
    scale += scl;
    updated = true;
    boundsOutdated = true;
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
}

void TransformComponent::setScale(const gsl::vec3 &scl)
{
    scale = scl;
    updated = true;
    boundsOutdated = true;
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

    rotationArr.insert(0, QJsonValue(static_cast<double>(rotation.i)));
    rotationArr.insert(1, QJsonValue(static_cast<double>(rotation.j)));
    rotationArr.insert(2, QJsonValue(static_cast<double>(rotation.k)));
    rotationArr.insert(3, QJsonValue(static_cast<double>(rotation.s)));
    parentObj.insert("Rotation", rotationArr);

    scaleArr.insert(0, QJsonValue(static_cast<double>(scale.x)));
    scaleArr.insert(1, QJsonValue(static_cast<double>(scale.y)));
    scaleArr.insert(2, QJsonValue(static_cast<double>(scale.z)));
    parentObj.insert("Scale", scaleArr);

    return parentObj;
}

void TransformComponent::fromJSON(QJsonObject object)
{
    auto childsArray = object["Children"].toArray();
    for(auto child : childsArray)
    {
        children.push_back(static_cast<unsigned>(child.toInt()));
    }

    auto positionArray = object["Position"].toArray();
    position.x = static_cast<float>(positionArray[0].toDouble());
    position.y = static_cast<float>(positionArray[1].toDouble());
    position.z = static_cast<float>(positionArray[2].toDouble());

    auto rotationArray = object["Rotation"].toArray();
    rotation.i = static_cast<float>(rotationArray[0].toDouble());
    rotation.j = static_cast<float>(rotationArray[1].toDouble());
    rotation.k = static_cast<float>(rotationArray[2].toDouble());
    rotation.s = static_cast<float>(rotationArray[3].toDouble());

    auto scaleArray = object["Scale"].toArray();
    scale.x = static_cast<float>(scaleArray[0].toDouble());
    scale.y = static_cast<float>(scaleArray[1].toDouble());
    scale.z = static_cast<float>(scaleArray[2].toDouble());

    updated = true;
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
        load(file.toStdString());
    }
}

bool ScriptComponent::load(const std::string& file)
{
    if(!updatedEntityID)
    {
        engine->globalObject().setProperty("entityID", entityId);
        updatedEntityID = true;
    }

    if(!file.size())
    {
        return false;
    }
    QFile scriptFile(QString::fromStdString(file));
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open script: " << QString::fromStdString(file);
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    auto value = engine->evaluate(contents, QString::fromStdString(file));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    filePath = file;
    return true;
}

bool ScriptComponent::call(const std::string& function)
{
    if(!filePath.size())
        return false;

    QJSValue value = engine->evaluate(QString::fromStdString(function), QString::fromStdString(filePath));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    value.call();
    return true;
}

bool ScriptComponent::call(const std::string& function, QJSValueList params)
{
    if(!filePath.size())
        return false;

    QJSValue value = engine->evaluate(QString::fromStdString(function), QString::fromStdString(filePath));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    value.call(params);
    return true;
}

bool ScriptComponent::execute(QString function, QString contents, QString fileName)
{
    if(!function.size() || !contents.size() || !fileName.size())
        return false;

    if(!updatedEntityID)
    {
        engine->globalObject().setProperty("entityID", entityId);
        updatedEntityID = true;
    }


    QJSValue value = engine->evaluate(contents, fileName);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    value = engine->evaluate(function);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    value.call();
    return true;
}

std::pair<gsl::vec3, gsl::vec3> ColliderComponent::Bounds::minMax() const {
    return {centre - extents * 0.5f, centre + extents * 0.5f};
}

QJsonObject Component::toJSON()
{
    QJsonObject returnObject;
    returnObject.insert("Valid", QJsonValue(valid));
    returnObject.insert("ComponentType", QJsonValue(static_cast<int>(type)));
    return returnObject;
}

void Component::fromJSON(QJsonObject object)
{
    valid = object["Valid"].toBool();
}

QJsonObject EntityInfo::toJSON()
{
    auto parentObj = Component::toJSON();
    parentObj.insert("Name", QJsonValue(name.c_str()));
    return parentObj;
}

void EntityInfo::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    name = object["Name"].toString().toStdString();
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
    velocity.x = static_cast<float>(velocityArr[0].toDouble());
    velocity.y = static_cast<float>(velocityArr[1].toDouble());
    velocity.z = static_cast<float>(velocityArr[2].toDouble());

    auto accelerationArr = object["Acceleration"].toArray();
    acceleration.x = static_cast<float>(velocityArr[0].toDouble());
    acceleration.y = static_cast<float>(velocityArr[1].toDouble());
    acceleration.z = static_cast<float>(velocityArr[2].toDouble());

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
        meshData = *ResourceManager::instance()->getMesh(meshName);
        meshData.mRenderType = static_cast<GLenum>(meshDataObj["RenderType"].toInt());
    }

    // Material data

    auto materialObj = object["MaterialData"].toObject();
    auto shaderName = materialObj["Shader"].toString();
    if(shaderName.size() && shaderName != "None")
    {
        mMaterial.loadShaderWithParameters(ResourceManager::instance()->getShader(shaderName.toStdString()));
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

    parentObj.insert("CurrentActive", QJsonValue(isCurrentActive));
    parentObj.insert("FramebufferTarget", QJsonValue(static_cast<int>(framebufferTarget)));
    parentObj.insert("Pitch", QJsonValue(static_cast<double>(pitch)));
    parentObj.insert("Yaw", QJsonValue(static_cast<double>(yaw)));

    return parentObj;
}

void CameraComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    isCurrentActive = object["CurrentActive"].toBool();
    framebufferTarget = static_cast<unsigned>(object["FramebufferTarget"].toInt());
    pitch = static_cast<float>(object["Pitch"].toDouble());
    yaw = static_cast<float>(object["Yaw"].toDouble());
}

QJsonObject InputComponent::toJSON()
{
    auto parentObj = Component::toJSON();

    parentObj.insert("CurrentlyControlled", QJsonValue(isCurrentlyControlled));

    return parentObj;
}

void InputComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    isCurrentlyControlled = object["CurrentlyControlled"].toBool();
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
        SoundManager::createSource(this, name);
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

    qDebug() << "NOTE: Collider to JSON not implemented yet.";

    return parentObj;
}

void ColliderComponent::fromJSON(QJsonObject object)
{
    Component::fromJSON(object);

    qDebug() << "NOTE: Collider from JSON not implemented yet.";

}
