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

    if(name == "transform")
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
        return QJSValue();

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

    if(name == "transform")
    {
        TransformComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "mesh")
    {
        MeshComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "physics")
    {
        PhysicsComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "camera")
    {
        CameraComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "input")
    {
        InputComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "sound")
    {
        SoundComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "pointLight")
    {
        PointLightComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "directionalLight")
    {
        DirectionalLightComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "spotLight")
    {
        SpotLightComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "script")
    {
        ScriptComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }
    else if(name == "collider")
    {
        ColliderComponent comp{};
        auto JSON = comp.toJSON();
        JSON.insert("ID", QJsonValue(static_cast<int>(id)));
        return mEngine->toScriptValue(JSON);
    }

    return QJSValue();
}
