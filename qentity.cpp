#include "qentity.h"
#include "entitymanager.h"
#include "componentdata.h"
#include <QJsonDocument>
#include "scriptsystem.h"
#include "soundmanager.h"

QEntity::QEntity(unsigned int _ID, QObject* parent)
    :  QObject(parent), mID(_ID)
{
}

QJSValue QEntity::getComponent(const QString &name)
{
    QJSValue value;

    if(name == "transform")
        value = ScriptSystem::get()->getComponent<TransformComponent>(mID);
    else if(name == "mesh")
        value = ScriptSystem::get()->getComponent<MeshComponent>(mID);
    else if(name == "physics")
        value = ScriptSystem::get()->getComponent<PhysicsComponent>(mID);
    else if(name == "camera")
        value = ScriptSystem::get()->getComponent<CameraComponent>(mID);
    else if(name == "input")
        value = ScriptSystem::get()->getComponent<InputComponent>(mID);
    else if(name == "sound")
        value = ScriptSystem::get()->getComponent<SoundComponent>(mID);
    else if(name == "pointLight")
        value = ScriptSystem::get()->getComponent<PointLightComponent>(mID);
    else if(name == "directionalLight")
        value = ScriptSystem::get()->getComponent<DirectionalLightComponent>(mID);
    else if(name == "spotLight")
        value = ScriptSystem::get()->getComponent<SpotLightComponent>(mID);
    else if(name == "script")
        value = ScriptSystem::get()->getComponent<ScriptComponent>(mID);
    else if(name == "collider")
        value = ScriptSystem::get()->getComponent<ColliderComponent>(mID);

    return value;
}

QJSValue QEntity::addComponent(const QString &name)
{
    QJSValue value;

    if(name == "transform")
        value = ScriptSystem::get()->addComponent<TransformComponent>(mID);
    else if(name == "mesh")
        value = ScriptSystem::get()->addComponent<MeshComponent>(mID);
    else if(name == "physics")
        value = ScriptSystem::get()->addComponent<PhysicsComponent>(mID);
    else if(name == "camera")
        value = ScriptSystem::get()->addComponent<CameraComponent>(mID);
    else if(name == "input")
        value = ScriptSystem::get()->addComponent<InputComponent>(mID);
    else if(name == "sound")
        value = ScriptSystem::get()->addComponent<SoundComponent>(mID);
    else if(name == "pointLight")
        value = ScriptSystem::get()->addComponent<PointLightComponent>(mID);
    else if(name == "directionalLight")
        value = ScriptSystem::get()->addComponent<DirectionalLightComponent>(mID);
    else if(name == "spotLight")
        value = ScriptSystem::get()->addComponent<SpotLightComponent>(mID);
    else if(name == "script")
        value = ScriptSystem::get()->addComponent<ScriptComponent>(mID);
    else if(name == "collider")
        value = ScriptSystem::get()->addComponent<ColliderComponent>(mID);

    return value;
}

void QEntity::playSound()
{
    if(auto soundComp = World::getWorld().getEntityManager()->getComponent<SoundComponent>(mID))
    {
        auto source = soundComp->mSource;
        if(source > -1)
        {
            SoundManager::get().play(static_cast<unsigned>(source));
        }
    }
}
