#include "scriptsystem.h"
#include <QFile>
#include <QTextStream>
#include "entitymanager.h"
#include "componentdata.h"
#include "qentity.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QPoint>

// For HitInfo struct
#include "physicssystem.h"

void ScriptSystem::beginPlay(std::vector<ScriptComponent>& comps)
{
    initializeHelperFuncs();

    for(auto& comp : comps)
    {
        if(comp.filePath.size())
        {
            // To catch changes to scripts after they have been loaded
            load(comp, comp.filePath);

            call(comp, "beginPlay");
        }
    }
}

void ScriptSystem::tick(float deltaTime, std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(comp.filePath.size())
        {
            mDeltaTime = deltaTime;
            QJSValueList list;
            list << deltaTime;
            call(comp, "tick", list);
        }
    }
}

void ScriptSystem::endPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(comp.filePath.size())
        {
            call(comp, "endPlay");
        }
    }
}

void ScriptSystem::runKeyPressedEvent(ScriptComponent& comp, const std::vector<QString>& keys)
{
    if(!comp.filePath.size() || !keys.size())
        return;

    QJSValueList list;
    auto array = comp.engine->newArray(static_cast<unsigned>(keys.size()));
    for(unsigned i = 0; i < keys.size(); ++i)
    {
        array.setProperty(i, keys[i]);
    }
    list << array;
    call(comp, "inputPressed", list);
}

void ScriptSystem::runKeyReleasedEvent(ScriptComponent &comp, const std::vector<QString> &keys)
{
    if(!comp.filePath.size() || !keys.size())
        return;

    QJSValueList list;
    auto array = comp.engine->newArray(static_cast<unsigned>(keys.size()));
    for(unsigned i = 0; i < keys.size(); ++i)
    {
        array.setProperty(i, keys[i]);
    }
    list << array;
    call(comp, "inputReleased", list);
}

void ScriptSystem::runMouseOffsetEvent(ScriptComponent &comp, const QPoint& point)
{
    if(!comp.filePath.size())
        return;

    QJSValueList list;
    auto array = comp.engine->newArray(2);
    array.setProperty(0, point.x());
    array.setProperty(1, point.y());
    list << array;
    call(comp, "mouseMoved", list);
}

void ScriptSystem::runHitEvents(std::vector<ScriptComponent>& comps, std::vector<HitInfo> hitInfos)
{
    if(!comps.size() || !hitInfos.size())
        return;

    for(auto& hitInfo : hitInfos)
    {
        for(unsigned i = 0; i < comps.size(); ++i)
        {
            if(!comps[i].filePath.size())
                continue;

            if(comps[i].entityId == hitInfo.eID)
            {
                QJsonObject hitJSON;
                hitJSON.insert("ID", static_cast<int>(hitInfo.eID));
                hitJSON.insert("hitPoint", hitInfo.hitPoint.toJSON());
                hitJSON.insert("velocity", hitInfo.velocity.toJSON());
                hitJSON.insert("collidingNormal", hitInfo.collidingNormal.toJSON());

                QJSValueList list;
                QJsonDocument doc(hitJSON);
                list << comps[i].engine->toScriptValue(hitJSON);
                call(comps[i], "onHit", list);
            }
        }
    }
}

void ScriptSystem::updateJSComponents(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(comp.filePath.size())
        {
            updateJSComponents(comp);
        }
    }
}

QString ScriptSystem::checkError(QJSValue value)
{
    QString lineNumber = QString::number(value.property("lineNumber").toInt());
    QString valueString = value.toString();
    QString error("Uncaught exception at line" + lineNumber  + ":" + valueString );
    qDebug() << error;
    return error;
}

QEntity* ScriptSystem::getEntityWrapper(unsigned int entity)
{
    return new QEntity(entity, currentComp->engine, entityManager.get(), this);
}

bool ScriptSystem::load(ScriptComponent& comp, const std::string& file)
{
    currentComp = &comp;

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("me", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    if(!file.size())
    {
        return false;
    }

    QFile scriptFile(QString::fromStdString(file));
    if(!scriptFile.exists())
    {
        qDebug() << "Script file (" + QString::fromStdString(file) + ") does not exist!";
        return false;
    }
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open script: " << QString::fromStdString(file);
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    contents.prepend(helperFuncs);
    scriptFile.close();
    auto value = comp.engine->evaluate(contents, QString::fromStdString(file));
    if(value.isError())
    {
        checkError(value);
        return false;
    }

    QFileInfo info(QString::fromStdString(file));
    comp.filePath = gsl::scriptsFilePath + info.baseName().toStdString() + ".js";
    currentComp = nullptr;
    return true;
}

void ScriptSystem::call(ScriptComponent& comp, const std::string& function)
{
    if(!comp.filePath.size())
        return;

    currentComp = &comp;
    currentFileName = QString::fromStdString(comp.filePath);

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("me", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    QJSValue value = comp.engine->evaluate(QString::fromStdString(function), currentFileName);
    if(value.isError())
    {
        checkError(value);
        return;
    }

    value.call();

    updateCPPComponents();
    currentComp = nullptr;
    return;
}

void ScriptSystem::call(ScriptComponent &comp, const std::string& function, QJSValueList params)
{
    if(!comp.filePath.size())
        return;

    currentComp = &comp;
    currentFileName = QString::fromStdString(comp.filePath);

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("me", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    QJSValue value = comp.engine->evaluate(QString::fromStdString(function), currentFileName);
    if(value.isError())
    {
        checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return;
    }

    value.call(params);

    updateCPPComponents();
    currentComp = nullptr;
    return;
}

QJSValue ScriptSystem::call(const std::string& function)
{
    if(currentComp && currentComp->JSEntity)
    {
        QJSValue value = currentComp->engine->evaluate(QString::fromStdString(function), currentFileName);
        if(value.isError())
        {
            checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }

        auto returnValue = value.call();

        if(returnValue.isError())
        {
            checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }
        return returnValue;
    }
    return false;
}

QJSValue ScriptSystem::call(const std::string& function, QJSValueList params)
{
    if(currentComp && currentComp->JSEntity)
    {
        QJSValue value = currentComp->engine->evaluate(QString::fromStdString(function), currentFileName);
        if(value.isError())
        {
            checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }

        auto returnValue = value.call(params);
        if(value.isError())
        {
            checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }

        return returnValue;
    }
    return false;
}

bool ScriptSystem::execute(ScriptComponent& comp, QString function, QString contents, QString fileName)
{
    if(!function.size() || !contents.size() || !fileName.size())
        return false;

    currentComp = &comp;
    currentFileName = fileName;

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("me", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    QJSValue value = comp.engine->evaluate(contents, fileName);
    if(value.isError())
    {
        checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return false;
    }

    value = comp.engine->evaluate(function);
    if(value.isError())
    {
        checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return false;
    }

    value = value.call();
    if(value.isError())
    {
        checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return false;
    }

    updateCPPComponents();
    comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    currentComp = nullptr;
    return true;
}

QObject* ScriptSystem::spawnCube()
{
    auto entity = entityManager->createCube();
    return getEntityWrapper(entity);
}

QObject *ScriptSystem::spawnEntity()
{
    return getEntityWrapper(entityManager->createEntity());
}

QJSValue ScriptSystem::getAllEntityIDsByComponent(const QString& name)
{
    if(!currentComp)
        return 0;

    std::vector<int> IDs;
    if(name == "transform")
    {
        auto components = entityManager->getTransformComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "mesh")
    {
        auto components = entityManager->getMeshComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "physics")
    {
        auto components = entityManager->getPhysicsComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "camera")
    {
        auto components = entityManager->getCameraComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "input")
    {
        auto components = entityManager->getInputComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "sound")
    {
        auto components = entityManager->getSoundComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "pointLight")
    {
        auto components = entityManager->getPointLightComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "directionalLight")
    {
        auto components = entityManager->getDirectionalLightComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "spotLight")
    {
        auto components = entityManager->getSpotLightComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "script")
    {
        auto components = entityManager->getScriptComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }
    else if(name == "collider")
    {
        auto components = entityManager->getColliderComponents();
        for(auto& comp : components)
        {
            IDs.emplace_back(comp.entityId);
        }
    }

    return currentComp->engine->toScriptValue(IDs);
}

QJSValue ScriptSystem::getAllEntityIDs()
{
    if(!currentComp)
        return 0;

    std::vector<int> IDs;
    for(auto comp : entityManager->getEntityInfos())
    {
        IDs.emplace_back(comp.entityId);
    }
    return currentComp->engine->toScriptValue(IDs);
}

QObject* ScriptSystem::getEntity(unsigned int id)
{
    if(!currentComp)
        return nullptr;

    return getEntityWrapper(id);
}

void ScriptSystem::updateCPPComponents()
{
    if(!currentComp)
        return;

    std::vector<QJsonObject> objects;
    auto componentArray = currentComp->engine->globalObject().property("accessedComponents");
    if(!componentArray.isUndefined() && !componentArray.isNull())
    {
        auto length = componentArray.property("length").toInt();
        if(length > 0)
        {
            for(unsigned i = 0; i < static_cast<unsigned>(length); ++i)
            {
                objects.push_back(QJsonValue::fromVariant(currentComp->engine->globalObject().property("accessedComponents").property(i).toVariant()).toObject());
            }

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
                    comp = entityManager->getComponent<EntityInfo>(static_cast<unsigned>(object["ID"].toInt()));
                    break;
               }
               case ComponentType::Mesh:
               {
                    comp = entityManager->getComponent<MeshComponent>(static_cast<unsigned>(object["ID"].toInt()));
                    break;
               }
               case ComponentType::Transform:
               {
                   comp = entityManager->getComponent<TransformComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Camera:
               {
                   comp = entityManager->getComponent<CameraComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Physics:
               {
                   comp = entityManager->getComponent<PhysicsComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Input:
               {
                   comp = entityManager->getComponent<InputComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Sound:
               {
                   comp = entityManager->getComponent<SoundComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::LightSpot:
               {
                   comp = entityManager->getComponent<SpotLightComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::LightPoint:
               {
                   comp = entityManager->getComponent<PointLightComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::LightDirectional:
               {
                   comp = entityManager->getComponent<DirectionalLightComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Script:
               {
                   comp = entityManager->getComponent<ScriptComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Collider:
               {
                   comp = entityManager->getComponent<ColliderComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               }

               if(comp)
               {
                   object.remove("ID");
                   // Check if they are different
                   // If they are different this component was modified in JS
                   // and we need to update the C++ version
                   auto oldJson = comp->toJSON();
                   if(object != oldJson)
                   {
                       comp->fromJSON(object);
                   }
               }
            }
        }
    }
}

void ScriptSystem::updateJSComponents(ScriptComponent& comp)
{
    if(!comp.filePath.size())
        return;

    auto componentArray = comp.engine->globalObject().property("accessedComponents");
    if(!componentArray.isUndefined() && !componentArray.isNull())
    {
        auto length = componentArray.property("length").toInt();
        if(length > 0)
        {
            for(unsigned i = 0; i < static_cast<unsigned>(length); ++i)
            {
                auto object = QJsonValue::fromVariant(componentArray.property(i).toVariant()).toObject();

                if(object.isEmpty())
                    continue;

                object.remove("ID");
                // Get component that matches the type
               Component* component;
               switch (static_cast<ComponentType>(object["ComponentType"].toInt()))
               {
               case ComponentType::Other:
               {
                    component = entityManager->getComponent<EntityInfo>(static_cast<unsigned>(object["ID"].toInt()));
                    break;
               }
               case ComponentType::Mesh:
               {
                    component = entityManager->getComponent<MeshComponent>(static_cast<unsigned>(object["ID"].toInt()));
                    break;
               }
               case ComponentType::Transform:
               {
                   component = entityManager->getComponent<TransformComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Camera:
               {
                   component = entityManager->getComponent<CameraComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Physics:
               {
                   component = entityManager->getComponent<PhysicsComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Input:
               {
                   component = entityManager->getComponent<InputComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Sound:
               {
                   component = entityManager->getComponent<SoundComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::LightSpot:
               {
                   component = entityManager->getComponent<SpotLightComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::LightPoint:
               {
                   component = entityManager->getComponent<PointLightComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::LightDirectional:
               {
                   component = entityManager->getComponent<DirectionalLightComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Script:
               {
                   component = entityManager->getComponent<ScriptComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               case ComponentType::Collider:
               {
                   component = entityManager->getComponent<ColliderComponent>(static_cast<unsigned>(object["ID"].toInt()));
                   break;
               }
               }

               if(component)
               {
                   // Check if they are different
                   // If they are different this component was modified in C++
                   // and we need to update the JS version
                   auto newJson = component->toJSON();
                   if(object != newJson)
                   {
                       comp.engine->globalObject().property("accessedComponents").setProperty(i, comp.engine->toScriptValue(newJson));
                   }
               }
            }
        }
    }
}

void ScriptSystem::initializeHelperFuncs()
{
    QFile file(QString::fromStdString("../INNgine2019/JSHelperFuncs.js"));
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR Script System: Failed to find JSHelperFuncs.js!";
        return;
    }

    helperFuncs = file.readAll();
    file.close();
}
