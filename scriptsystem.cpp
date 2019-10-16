#include "scriptsystem.h"
#include <QFile>
#include <QTextStream>
#include "entitymanager.h"
#include "componentdata.h"
#include "qentity.h"
#include <QFileInfo>

void ScriptSystem::beginPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(comp.filePath.size())
        {
            load(comp, comp.filePath);
            call(comp, "beginPlay");
        }
    }
}

void ScriptSystem::tick(float deltaTime, std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        QJSValueList list;
        list << deltaTime;
        call(comp, "tick", list);
    }
}

void ScriptSystem::endPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
       call(comp, "endPlay");
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

bool ScriptSystem::load(ScriptComponent &comp, const std::string &file)
{
    currentComp = &comp;

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("entity", comp.engine->newQObject(comp.JSEntity));
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
    contents.prepend(ScriptSystem::get()->getEntityHelperFunctions());
    scriptFile.close();
    auto value = comp.engine->evaluate(contents, QString::fromStdString(file));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    QFileInfo info(QString::fromStdString(file));
    comp.filePath = gsl::scriptsFilePath + info.baseName().toStdString() + ".js";
    currentComp = nullptr;
    return true;
}

void ScriptSystem::call(ScriptComponent& comp, const std::string &function)
{
    if(!comp.filePath.size())
        return;

    currentComp = &comp;
    currentFileName = QString::fromStdString(comp.filePath);

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("entity", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    QJSValue value = comp.engine->evaluate(QString::fromStdString(function), currentFileName);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return;
    }

    currentComp = &comp;
    value.call();

    checkForModifiedComponents();
    currentComp = nullptr;
    return;
}

QJSValue ScriptSystem::call(const std::string &function)
{
    if(currentComp && currentComp->JSEntity)
    {
        QJSValue value = currentComp->engine->evaluate(QString::fromStdString(function), currentFileName);
        if(value.isError())
        {
            ScriptSystem::get()->checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }

        auto returnValue = value.call();

        if(returnValue.isError())
        {
            ScriptSystem::get()->checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }
        return returnValue;
    }
    return false;
}

void ScriptSystem::call(ScriptComponent &comp, const std::string &function, QJSValueList params)
{
    if(!comp.filePath.size())
        return;

    currentComp = &comp;
    currentFileName = QString::fromStdString(comp.filePath);

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
        comp.engine->globalObject().setProperty("entity", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    QJSValue value = comp.engine->evaluate(QString::fromStdString(function), currentFileName);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return;
    }

    value.call(params);

    checkForModifiedComponents();
    currentComp = nullptr;
    return;
}

QJSValue ScriptSystem::call(const std::string &function, QJSValueList params)
{
    if(currentComp && currentComp->JSEntity)
    {
        QJSValue value = currentComp->engine->evaluate(QString::fromStdString(function), currentFileName);
        if(value.isError())
        {
            ScriptSystem::get()->checkError(value);
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());
            return false;
        }

        auto returnValue = value.call(params);
        if(value.isError())
        {
            ScriptSystem::get()->checkError(value);
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
        comp.engine->globalObject().setProperty("entity", comp.engine->newQObject(comp.JSEntity));
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    }

    QJSValue value = comp.engine->evaluate(contents, fileName);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return false;
    }

    value = comp.engine->evaluate(function);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return false;
    }

    auto v = value.call();
    if(v.isError())
    {
        ScriptSystem::get()->checkError(v);
        comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
        return false;
    }

    checkForModifiedComponents();
    comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    currentComp = nullptr;
    return true;
}

void ScriptSystem::checkForModifiedComponents()
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
                objects.push_back(QJsonValue::fromVariant( currentComp->engine->globalObject().property("accessedComponents").property(i).toVariant()).toObject());
            }

            //*** Remove this for caching (but reintroduces bug where JS will override accessed components even when C++ has made changes.
            currentComp->engine->globalObject().setProperty("accessedComponents", currentComp->engine->newArray());

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
                   // and we need to update it
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

QObject* ScriptSystem::spawnCube(float x, float y, float z)
{
    auto entity = entityManager->createCube();
    if(x != 0.f || y != 0.f || z != 0.f)
    {
        entityManager->setTransformPos(entity, gsl::vec3(x, y, z));
    }
    return getEntityWrapper(entity);
}

QObject *ScriptSystem::spawnEntity()
{
    return getEntityWrapper(entityManager->createEntity());
}



