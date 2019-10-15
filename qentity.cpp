#include "qentity.h"
#include "qcomponentdata.h"
#include <QJsonDocument>

QEntity::QEntity(unsigned int _ID, QJSEngine* engine, EntityManager* _entityManager, QObject* parent)
    :  QObject(parent), entityManager(_entityManager), mEngine(engine), mID(_ID)
{

}

void QEntity::updateComponents(std::vector<QJsonObject> objects)
{
    if(!objects.size())
        return;

    // Iterate through accessed components

    for(auto object : objects)
    {
        // Get component that matches the type
       Component* comp;
       switch (static_cast<ComponentType>(object["ComponentType"].toInt()))
       {
       case ComponentType::Other:
       {
            comp = entityManager->getComponent<EntityInfo>(mID);
            break;
       }
       case ComponentType::Mesh:
       {
            comp = entityManager->getComponent<MeshComponent>(mID);
            break;
       }
       case ComponentType::Transform:
       {
           comp = entityManager->getComponent<TransformComponent>(mID);
           break;
       }
       case ComponentType::Camera:
       {
           comp = entityManager->getComponent<CameraComponent>(mID);
           break;
       }
       case ComponentType::Physics:
       {
           comp = entityManager->getComponent<PhysicsComponent>(mID);
           break;
       }
       case ComponentType::Input:
       {
           comp = entityManager->getComponent<InputComponent>(mID);
           break;
       }
       case ComponentType::Sound:
       {
           comp = entityManager->getComponent<SoundComponent>(mID);
           break;
       }
       case ComponentType::LightSpot:
       {
           comp = entityManager->getComponent<SpotLightComponent>(mID);
           break;
       }
       case ComponentType::LightPoint:
       {
           comp = entityManager->getComponent<PointLightComponent>(mID);
           break;
       }
       case ComponentType::LightDirectional:
       {
           comp = entityManager->getComponent<DirectionalLightComponent>(mID);
           break;
       }
       case ComponentType::Script:
       {
           comp = entityManager->getComponent<ScriptComponent>(mID);
           break;
       }
       case ComponentType::Collider:
       {
           comp = entityManager->getComponent<ColliderComponent>(mID);
           break;
       }
       }

       if(comp)
       {
           // Check if they are different
           // If they are different this component was modified in JS
           // and we need to update it
           auto oldJson = comp->toJSON();
           if(object != oldJson)
           {
               comp->fromJSON(object);
           }
       }
    }
}

QJSValue QEntity::getComponent(const QString& name)
{
    Component* component = nullptr;

    if(name == "info")
        component = entityManager->getComponent<EntityInfo>(mID);
    else if(name == "transform")
        component = entityManager->getComponent<TransformComponent>(mID);
    else if(name == "mesh")
        component = entityManager->getComponent<MeshComponent>(mID);
    else if(name == "physics")
        component = entityManager->getComponent<PhysicsComponent>(mID);
    else if(name == "camera")
        component = entityManager->getComponent<CameraComponent>(mID);
    else if(name == "input")
        component = entityManager->getComponent<InputComponent>(mID);
    else if(name == "sound")
        component = entityManager->getComponent<SoundComponent>(mID);
    else if(name == "pointLight")
        component = entityManager->getComponent<PointLightComponent>(mID);
    else if(name == "directionalLight")
        component = entityManager->getComponent<DirectionalLightComponent>(mID);
    else if(name == "spotLight")
        component = entityManager->getComponent<SpotLightComponent>(mID);
    else if(name == "script")
        component = entityManager->getComponent<ScriptComponent>(mID);
    else if(name == "collider")
        component = entityManager->getComponent<ColliderComponent>(mID);

    // Return 0 if nothing was found
    if(!component)
        return 0;

    return mEngine->toScriptValue(component->toJSON());
}

