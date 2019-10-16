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
        call(comp, "beginPlay");
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
    return new QEntity(entity, engine, entityManager.get(), this);
}

bool ScriptSystem::load(ScriptComponent& comp, const std::string& file)
{
    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
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
    contents.prepend(addCompFunc);
    contents.prepend(getCompFunc);
    scriptFile.close();
    auto value = engine->evaluate(contents, QString::fromStdString(file));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        return false;
    }

    QFileInfo info(QString::fromStdString(file));
    comp.filePath = gsl::scriptsFilePath + info.baseName().toStdString() + ".js";
    return true;
}

bool ScriptSystem::call(ScriptComponent& comp, const std::string& function)
{
    if(!comp.filePath.size())
        return false;

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
    }

    QJSValue value = engine->evaluate(QString::fromStdString(function), QString::fromStdString(comp.filePath));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        engine->globalObject().setProperty("accessedComponents", engine->newArray());
        return false;
    }

    currentQEntity = comp.JSEntity;
    engine->globalObject().setProperty("entity", engine->newQObject(comp.JSEntity));
    value.call();

    checkForModifiedComponents(comp);
    return true;
}

bool ScriptSystem::call(ScriptComponent& comp, const std::string& function, QJSValueList params)
{
    if(!comp.filePath.size())
        return false;

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
    }

    QJSValue value = engine->evaluate(QString::fromStdString(function), QString::fromStdString(comp.filePath));
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        engine->globalObject().setProperty("accessedComponents", engine->newArray());
        return false;
    }

    currentQEntity = comp.JSEntity;
    engine->globalObject().setProperty("entity", engine->newQObject(comp.JSEntity));
    value.call(params);

    checkForModifiedComponents(comp);
    return true;
}

bool ScriptSystem::execute(ScriptComponent& comp, QString function, QString contents, QString fileName)
{
    if(!function.size() || !contents.size() || !fileName.size())
        return false;

    if(!comp.JSEntity)
    {
        comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
    }

    QJSValue value = engine->evaluate(contents, fileName);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        engine->globalObject().setProperty("accessedComponents", engine->newArray());
        return false;
    }

    value = engine->evaluate(function);
    if(value.isError())
    {
        ScriptSystem::get()->checkError(value);
        engine->globalObject().setProperty("accessedComponents", engine->newArray());
        return false;
    }

    currentQEntity = comp.JSEntity;
    engine->globalObject().setProperty("entity", engine->newQObject(comp.JSEntity));
    auto v = value.call();
    if(v.isError())
    {
        ScriptSystem::get()->checkError(v);
        engine->globalObject().setProperty("accessedComponents", engine->newArray());
        return false;
    }

    checkForModifiedComponents(comp);

    return true;
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

QObject* ScriptSystem::spawnEntity()
{
    auto entity = entityManager->createEntity();
    return getEntityWrapper(entity);
}

ScriptSystem::ScriptSystem()
{
    engine = new QJSEngine();
    engine->installExtensions(QJSEngine::ConsoleExtension);
    engine->globalObject().setProperty("engine", engine->newQObject(this));
    engine->globalObject().setProperty("accessedComponents", engine->newArray());
}

//void ScriptSystem::checkForModifiedComponents(ScriptComponent& comp)
//{
//    std::vector<QJsonObject> objects;
//    auto componentArray = comp.engine->globalObject().property("accessedComponents");
//    if(!componentArray.isUndefined() && !componentArray.isNull())
//    {
//        auto length = componentArray.property("length").toInt();
//        if(length > 0)
//        {
//            for(unsigned i = 0; i < static_cast<unsigned>(length); ++i)
//            {
//                objects.push_back(QJsonValue::fromVariant(comp.engine->globalObject().property("accessedComponents").property(i).toVariant()).toObject());
//            }
//            comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
//            comp.JSEntity->updateComponents(objects);
//        }
//    }
//}

void ScriptSystem::checkForModifiedComponents(ScriptComponent& comp)
{
    std::vector<QJsonObject> objects;
    auto componentArray = engine->globalObject().property("accessedComponents");
    qDebug() << componentArray.toString();
    if(!componentArray.isUndefined() && !componentArray.isNull())
    {
        auto length = componentArray.property("length").toInt();
        if(length > 0)
        {
            for(unsigned i = 0; i < static_cast<unsigned>(length); ++i)
            {
                auto lol = engine->globalObject().property("accessedComponent").property(i);
                qDebug() << lol.toString();

            //    objects.push_back(QJsonValue::fromVariant(comp.engine->globalObject().property("accessedComponents").property(i).toVariant()).toObject());
            }
            engine->globalObject().setProperty("accessedComponents", engine->newArray());
//            comp.JSEntity->updateComponents(objects);

//            for(auto object : objects)
//            {
//                // Get component that matches the type
//               Component* comp;
//               switch (static_cast<ComponentType>(object["ComponentType"].toInt()))
//               {
//               case ComponentType::Other:
//               {
//                    comp = entityManager->getComponent<EntityInfo>(mID);
//                    break;
//               }
//               case ComponentType::Mesh:
//               {
//                    comp = entityManager->getComponent<MeshComponent>(mID);
//                    break;
//               }
//               case ComponentType::Transform:
//               {
//                   comp = entityManager->getComponent<TransformComponent>(mID);
//                   break;
//               }
//               case ComponentType::Camera:
//               {
//                   comp = entityManager->getComponent<CameraComponent>(mID);
//                   break;
//               }
//               case ComponentType::Physics:
//               {
//                   comp = entityManager->getComponent<PhysicsComponent>(mID);
//                   break;
//               }
//               case ComponentType::Input:
//               {
//                   comp = entityManager->getComponent<InputComponent>(mID);
//                   break;
//               }
//               case ComponentType::Sound:
//               {
//                   comp = entityManager->getComponent<SoundComponent>(mID);
//                   break;
//               }
//               case ComponentType::LightSpot:
//               {
//                   comp = entityManager->getComponent<SpotLightComponent>(mID);
//                   break;
//               }
//               case ComponentType::LightPoint:
//               {
//                   comp = entityManager->getComponent<PointLightComponent>(mID);
//                   break;
//               }
//               case ComponentType::LightDirectional:
//               {
//                   comp = entityManager->getComponent<DirectionalLightComponent>(mID);
//                   break;
//               }
//               case ComponentType::Script:
//               {
//                   comp = entityManager->getComponent<ScriptComponent>(mID);
//                   break;
//               }
//               case ComponentType::Collider:
//               {
//                   comp = entityManager->getComponent<ColliderComponent>(mID);
//                   break;
//               }
//               }

//               if(comp)
//               {
//                   // Check if they are different
//                   // If they are different this component was modified in JS
//                   // and we need to update it
//                   auto oldJson = comp->toJSON();
//                   if(object != oldJson)
//                   {
//                       comp->fromJSON(object);
//                   }
//               }
        }
    }
}

