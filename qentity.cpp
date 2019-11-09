#include "qentity.h"
#include "entitymanager.h"
#include "componentdata.h"
#include <QJsonDocument>

QEntity::QEntity(unsigned int _ID, QJSEngine* engine, EntityManager* _entityManager, QObject* parent)
    :  QObject(parent), entityManager(_entityManager), mEngine(engine), mID(_ID)
{
}

QJSValue QEntity::getComponent(const QString &name)
{
    QJSValueList list;
    list << name;
    list << mID;
    return ScriptSystem::get()->call("getComponent", list);
}

QJSValue QEntity::addComponent(const QString &name)
{
    QJSValueList list;
    list << name;
    list << mID;
    return ScriptSystem::get()->call("addComponent", list);
}

QJSValue QEntity::_getComponent(const QString& name, unsigned id)
{
    if(!id)
    {
        id = mID;
    }

    Component* component = nullptr;

    if(name == "info")
        component = entityManager->getComponent<EntityInfo>(id);
    else if(name == "transform")
        component = entityManager->getComponent<TransformComponent>(id);
    else if(name == "mesh")
        component = entityManager->getComponent<MeshComponent>(id);
    else if(name == "physics")
        component = entityManager->getComponent<PhysicsComponent>(id);
    else if(name == "camera")
        component = entityManager->getComponent<CameraComponent>(id);
    else if(name == "input")
        component = entityManager->getComponent<InputComponent>(id);
    else if(name == "sound")
        component = entityManager->getComponent<SoundComponent>(id);
    else if(name == "pointLight")
        component = entityManager->getComponent<PointLightComponent>(mID);
    else if(name == "directionalLight")
        component = entityManager->getComponent<DirectionalLightComponent>(id);
    else if(name == "spotLight")
        component = entityManager->getComponent<SpotLightComponent>(id);
    else if(name == "script")
        component = entityManager->getComponent<ScriptComponent>(id);
    else if(name == "collider")
        component = entityManager->getComponent<ColliderComponent>(id);

    // Return 0 if nothing was found
    if(!component)
        return 0;

    auto JSON = component->toJSON();
    JSON.insert("ID", QJsonValue(static_cast<int>(id)));
    return mEngine->toScriptValue(JSON);
}

QJSValue QEntity::_addComponent(const QString &name, unsigned id)
{
    if(!id)
    {
        id = mID;
    }

    Component* component = nullptr;

    if(name == "transform")
        component = entityManager->addComponent(id, ComponentType::Transform);
    else if(name == "mesh")
        component = entityManager->addComponent(id, ComponentType::Mesh);
    else if(name == "physics")
        component = entityManager->addComponent(id, ComponentType::Physics);
    else if(name == "camera")
        component = entityManager->addComponent(id, ComponentType::Camera);
    else if(name == "input")
        component = entityManager->addComponent(id, ComponentType::Input);
    else if(name == "sound")
        component = entityManager->addComponent(id, ComponentType::Sound);
    else if(name == "pointLight")
        component = entityManager->addComponent(id, ComponentType::LightPoint);
    else if(name == "directionalLight")
        component = entityManager->addComponent(id, ComponentType::LightDirectional);
    else if(name == "spotLight")
        component = entityManager->addComponent(id, ComponentType::LightSpot);
    else if(name == "script")
        component = entityManager->addComponent(id, ComponentType::Script);
    else if(name == "collider")
        component = entityManager->addComponent(id, ComponentType::Collider);

    // Return 0 if nothing was found
    if(!component)
        return 0;

    auto JSON = component->toJSON();
    JSON.insert("ID", QJsonValue(static_cast<int>(id)));
    return mEngine->toScriptValue(JSON);
}

void QEntity::rotateCamera(float x, float y)
{
    auto [camera, transform] = entityManager->getComponents<CameraComponent, TransformComponent>(mID);

    if(!camera || !transform)
        return;

    camera->yaw += x;
    camera->pitch += y;

    transform->setRotation(gsl::quat::lookAt(gsl::deg2radf(camera->pitch), gsl::deg2radf(camera->yaw)));
}

void QEntity::setMesh(const QString &name)
{
    if(!name.size())
        return;

    auto meshComp = entityManager->getComponent<MeshComponent>(mID);
    if(!meshComp)
        return;

    auto meshData = ResourceManager::instance()->getMesh(name.toStdString());
    if(!meshData)
        return;

    meshComp->meshData = *meshData;
}

void QEntity::setShader(const QString &name)
{
    if(!name.size())
        return;

    auto meshComp = entityManager->getComponent<MeshComponent>(mID);
    if(!meshComp)
        return;

    auto shader = ResourceManager::instance()->getShader(name.toStdString());
    if(!shader)
        return;

    meshComp->mMaterial.loadShaderWithParameters(shader);
}
