#include "scriptsystem.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "world.h"
#include "componentdata.h"
#include "qentity.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QPoint>
#include <cstring>
#include <QJSValueIterator>

// For HitInfo struct
#include "physicssystem.h"

#include "Instrumentor.h"


void ScriptSystem::update(std::vector<ScriptComponent> &scripts, std::vector<InputComponent> &inputs,
                          const std::vector<QString> &pressed, const std::vector<QString> &released,
                          const QPoint &point, std::vector<HitInfo> hitInfos, float deltaTime)
{
    PROFILE_FUNCTION();
    initializeHelperFuncs();
    auto inpIt{inputs.begin()};

    mDeltaTime = deltaTime;

    auto hitIt{hitInfos.begin()};
    if (hitIt != hitInfos.end())
    {
        std::sort(hitIt, hitInfos.end());
        hitIt = hitInfos.begin();
    }

    for (auto scriptIt{scripts.begin()}; scriptIt != scripts.end(); ++scriptIt)
    {
        PROFILE_SCOPE("Scriptloop");
        bool startedThisFrame = false;
        if (!scriptIt->valid || scriptIt->engine == nullptr || !scriptIt->filePath.size())
            continue;

        auto functions = scriptIt->engine->newArray(10);
        unsigned int i{0};
        std::vector<QJSValue> funcObjs;
        funcObjs.reserve(10);

        // Beginplay
        /* Note: This is called every frame, but only actually called on script components that this
         * has not yet been done to. This is to catch script components spawned from scripts
         * on runtime.
         */
        if (!scriptIt->beginplayRun)
        {
            PROFILE_SCOPE("Begin play");
            // To catch changes to script during runtime
            if(!load(*scriptIt, scriptIt->filePath))
            {
                qDebug() << "Failed to load" << scriptIt->filePath.c_str();
                continue;
            }

            scriptIt->beginplayRun = true;
            startedThisFrame = true;
            funcObjs.push_back(scriptIt->engine->newObject());
            funcObjs.back().setProperty("func", QJSValue{"beginPlay"});
            funcObjs.back().setProperty("params", QJSValue{});
            functions.setProperty(i, funcObjs.back());
            ++i;
        }

        if (!scriptIt->beginplayRun)
            continue;

        {
            PROFILE_SCOPE("Tick");
        // Tick
        funcObjs.push_back(scriptIt->engine->newObject());
        funcObjs.back().setProperty("func", QJSValue{"tick"});
        funcObjs.back().setProperty("params", QJSValue{});
        functions.setProperty(i, funcObjs.back());
        ++i;
        }

        // Input functions
        while (inpIt != inputs.end() && (!inpIt->valid || inpIt->entityId < scriptIt->entityId)) ++inpIt;

        if (inpIt != inputs.end() && inpIt->entityId == scriptIt->entityId && inpIt->controlledWhilePlaying)
        {

            if(!scriptIt->JSEntity)
            {
                initializeJSEntity(*scriptIt);
            }

            // Pressed key event
            if (pressed.size())
            {
                PROFILE_SCOPE("Pressed key");
                auto array = scriptIt->engine->newArray(static_cast<unsigned>(pressed.size()));
                for(unsigned i = 0; i < pressed.size(); ++i)
                {
                    array.setProperty(i, pressed[i]);
                }
                funcObjs.push_back(scriptIt->engine->newObject());
                funcObjs.back().setProperty("func", QJSValue{"inputPressed"});
                funcObjs.back().setProperty("params", array);
                functions.setProperty(i, funcObjs.back());
                ++i;
            }

            // Released key event
            if (released.size())
            {
                PROFILE_SCOPE("Released key");
                auto array = scriptIt->engine->newArray(static_cast<unsigned>(released.size()));
                for(unsigned i = 0; i < released.size(); ++i)
                {
                    array.setProperty(i, released[i]);
                }
                funcObjs.push_back(scriptIt->engine->newObject());
                funcObjs.back().setProperty("func", QJSValue{"inputReleased"});
                funcObjs.back().setProperty("params", array);
                functions.setProperty(i, funcObjs.back());
                ++i;
            }

            {
                PROFILE_SCOPE("Mouse movement");
            // Mouse movement event
            auto array = scriptIt->engine->newArray(2);
            array.setProperty(0, point.x());
            array.setProperty(1, point.y());
            funcObjs.push_back(scriptIt->engine->newObject());
            funcObjs.back().setProperty("func", QJSValue{"mouseMoved"});
            funcObjs.back().setProperty("params", array);
            functions.setProperty(i, funcObjs.back());
            ++i;
            }
        }


        // Hit events
        while (hitIt != hitInfos.end() && hitIt->eID < scriptIt->entityId) ++hitIt;

        /* Note: Physics system only reacts to first thing that it collides with
         * frame. A.k.a. one object can only collide with one other object each
         * frame and get information about that collision.
         * It would be easy to implement a workaround on this. Either by sending
         * in multiple hit results as an array or by running the same function
         * multiple times per hit function.
         */
        if (hitIt != hitInfos.end() && scriptIt->entityId == hitIt->eID)
        {
            PROFILE_SCOPE("Hit events");
            QJSValue val = scriptIt->engine->newObject();
            val.setProperty("ID", static_cast<int>(hitIt->collidingEID));
            auto arr = scriptIt->engine->newArray(3);
            for (unsigned int i{0}; i < 3; ++i)
                arr.setProperty(i, static_cast<double>(hitIt->hitPoint[i]));
            val.setProperty("hitPoint", arr);
            for (unsigned int i{0}; i < 3; ++i)
                arr.setProperty(i, static_cast<double>(hitIt->velocity[i]));
            val.setProperty("velocity", arr);
            for (unsigned int i{0}; i < 3; ++i)
                arr.setProperty(i, static_cast<double>(hitIt->collidingNormal[i]));
            val.setProperty("collidingNormal", arr);

            funcObjs.push_back(scriptIt->engine->newObject());
            funcObjs.back().setProperty("func", QJSValue{"onHit"});
            funcObjs.back().setProperty("params", val);
            functions.setProperty(i, funcObjs.back());
            ++i;
        }


        {
            PROFILE_SCOPE("Javascript");
        // Finally run functions from JavaScript:
        QJSValueList list;
        list << functions;
        auto changedComps = call(*scriptIt, "updateLoop", list);

        if (changedComps.isError())
            continue;

        // Remember to do this after all functions have run but before contructing objects
        if (startedThisFrame)
            cacheGlobalVariables(*scriptIt);


        // Update cpp comps
        // updateCPPComponent(*scriptIt, changedComps);
        }
    }
}

void ScriptSystem::beginPlay(std::vector<ScriptComponent>& comps)
{

    PROFILE_FUNCTION();
    // To catch changes to helper function script during runtime
    initializeHelperFuncs();

    for (auto scriptIt = comps.begin(); scriptIt != comps.end(); ++scriptIt)
    {
        if(scriptIt->filePath.size() && !scriptIt->beginplayRun)
        {
            // To catch changes to script during runtime
            if(!load(*scriptIt, scriptIt->filePath))
            {
                qDebug() << "Failed to load" << scriptIt->filePath.c_str();
                continue;
            }
            scriptIt->beginplayRun = true;
            call(*scriptIt, "beginPlay");
            cacheGlobalVariables(*scriptIt);
        }
    }
}

void ScriptSystem::tick(float deltaTime, std::vector<ScriptComponent>& comps)
{
    PROFILE_FUNCTION();
    mDeltaTime = deltaTime;
    for (auto scriptIt = comps.begin(); scriptIt != comps.end(); ++scriptIt)
    {
        if(scriptIt->filePath.size() && scriptIt->beginplayRun)
        {
            call(*scriptIt, "tick");
        }
    }
}

void ScriptSystem::endPlay(std::vector<ScriptComponent>& comps)
{

    for (auto scriptIt = comps.begin(); scriptIt != comps.end(); ++scriptIt)
    {
        if(scriptIt->filePath.size() && scriptIt->beginplayRun)
        {
            call(*scriptIt, "endPlay");
            scriptIt->beginplayRun = false;
        }
    }
}

void ScriptSystem::runKeyPressedEvent(std::vector<ScriptComponent>& scripts, std::vector<InputComponent>& inputs, const std::vector<QString>& keys)
{

    if(!keys.size())
        return;
    PROFILE_FUNCTION();
    for (auto inputIt = inputs.begin(); inputIt != inputs.end(); ++inputIt)
    {
        if(!inputIt->controlledWhilePlaying)
            continue;

        for (auto scriptIt = scripts.begin(); scriptIt != scripts.end(); ++scriptIt)
        {
            if(!scriptIt->filePath.size() || !scriptIt->beginplayRun)
                continue;

            if(!scriptIt->JSEntity)
            {
                initializeJSEntity(*scriptIt);
            }

            QJSValueList list;
            auto array = scriptIt->engine->newArray(static_cast<unsigned>(keys.size()));
            for(unsigned i = 0; i < keys.size(); ++i)
            {
                array.setProperty(i, keys[i]);
            }
            list << array;
            call(*scriptIt, "inputPressed", list);
        }
    }
}

void ScriptSystem::runKeyReleasedEvent(std::vector<ScriptComponent>& scripts, std::vector<InputComponent> &inputs, const std::vector<QString>& keys)
{

    if(!keys.size())
        return;
    PROFILE_FUNCTION();
    for (auto inputIt = inputs.begin(); inputIt != inputs.end(); ++inputIt)
    {
        if(!inputIt->controlledWhilePlaying)
            continue;

        for (auto scriptIt = scripts.begin(); scriptIt != scripts.end(); ++scriptIt)
        {
            if(!scriptIt->filePath.size() || !scriptIt->beginplayRun)
                continue;

            if(!scriptIt->JSEntity)
            {
                initializeJSEntity(*scriptIt);
            }

            QJSValueList list;
            auto array = scriptIt->engine->newArray(static_cast<unsigned>(keys.size()));
            for(unsigned i = 0; i < keys.size(); ++i)
            {
                array.setProperty(i, keys[i]);
            }
            list << array;
            call(*scriptIt, "inputReleased", list);
        }
    }
}

void ScriptSystem::runMouseOffsetEvent(std::vector<ScriptComponent> &scripts, std::vector<InputComponent> &inputs, const QPoint& point)
{
    PROFILE_FUNCTION();
    for (auto inputIt = inputs.begin(); inputIt != inputs.end(); ++inputIt)
    {
        if(!inputIt->controlledWhilePlaying)
            continue;

        for (auto scriptIt = scripts.begin(); scriptIt != scripts.end(); ++scriptIt)
        {
            if(!scriptIt->filePath.size() || !scriptIt->beginplayRun)
                continue;

            if(!scriptIt->JSEntity)
            {
                initializeJSEntity(*scriptIt);
            }

            QJSValueList list;
            auto array = scriptIt->engine->newArray(2);
            array.setProperty(0, point.x());
            array.setProperty(1, point.y());
            list << array;
            call(*scriptIt, "mouseMoved", list);
        }
    }
}

void ScriptSystem::runHitEvents(std::vector<ScriptComponent>& comps, std::vector<HitInfo> hitInfos)
{

    if(!comps.size() || !hitInfos.size())
        return;
    PROFILE_FUNCTION();

    for (auto it = hitInfos.begin(); it != hitInfos.end(); ++it)
    {
        QJsonObject hitJSON;
        hitJSON.insert("ID", static_cast<int>(it->collidingEID));
        hitJSON.insert("hitPoint", it->hitPoint.toJSON());
        hitJSON.insert("velocity", it->velocity.toJSON());
        hitJSON.insert("collidingNormal", it->collidingNormal.toJSON());

        QJSValueList list;
        QJsonDocument doc(hitJSON);

        for (auto scriptIt = comps.begin(); scriptIt != comps.end(); ++scriptIt)
        {
            if(!scriptIt->filePath.size() || !scriptIt->beginplayRun)
                continue;

            if(!scriptIt->JSEntity)
            {
                initializeJSEntity(*scriptIt);
            }

            if(scriptIt->entityId == it->eID)
            {
                list << scriptIt->engine->toScriptValue(hitJSON);
                call(*scriptIt, "onHit", list);
            }
        }
    }
}

void ScriptSystem::updateJSComponents(std::vector<ScriptComponent>& comps)
{
    PROFILE_FUNCTION();
    for (auto it = comps.begin(); it != comps.end(); ++it)
    {
        if(it->filePath.size())
        {
            updateJSComponent(*it);
        }
    }
}

void ScriptSystem::updateCPPComponents(std::vector<ScriptComponent> &comps)
{
    PROFILE_FUNCTION();
    for(auto& comp : comps)
    {
        if(comp.valid && comp.filePath.size())
        {
            updateCPPComponent(comp);
        }
    }
}

QString ScriptSystem::checkError(QJSValue value)
{

    QString lineNumber = QString::number(value.property("lineNumber").toInt());
    QString valueString = value.toString();
    QString error("Uncaught exception at line " +
                  lineNumber  +
                  " : " +
                  valueString +
                  " in script " +
                  currentFileName +
                  ", entity " +
                  QString::number(currentComp->entityId));
    qDebug() << error;
    return error;
}

QEntity* ScriptSystem::getEntityWrapper(unsigned int entity)
{

    return new QEntity(entity, currentComp->engine, World::getWorld().getEntityManager().get(), this);
}

void ScriptSystem::takeOutTheTrash(std::vector<ScriptComponent> &comps)
{
    PROFILE_FUNCTION();
    for (auto it = comps.begin(); it != comps.end(); ++it)
    {
        if (!(it->valid && it->engine))
            continue;

        auto cachedComps = it->engine->globalObject().property("accessedComponents");
        if (!cachedComps.isUndefined() && cachedComps.isArray())
        {
            auto variables = globalVariables[it->entityId];
            QJSValue variableArray = it->engine->newArray(static_cast<unsigned int>(variables.size()));
            for (unsigned int i{0}; i < variables.size(); ++i)
                variableArray.setProperty(i, variables[i]);

            QJSValueList args;
            args << variableArray;
            call(*it, "deleteUnusedVariables", args);
        }
    }
}

std::vector<QString> ScriptSystem::findGlobalsInFile(const std::string &file) const
{

    int scopeLevel{0};
    std::vector<QString> variables{};
    std::ifstream ifs{file, ifs.in};
    if (!ifs)
    {
        return {};
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        for (unsigned int s{0}, i{0}; i < line.length(); ++i)
        {
            if (line[i] == ';')
            {
                // Loops through prefix words in a c-style string literal array
                for (const auto& prefix : {"let", "var", "const"})
                {
                    // Finds the position of the first place where the prefix matches the string
                    auto pos = line.find(prefix, s, i - s);
                    if (pos != line.npos)
                    {
                        // Add characters to move to variable that is defined afterwards.
                        pos += std::strlen(prefix) + 1;

                        // Find start and end
                        auto start = line.find_first_not_of(" ", pos, i);

                        auto end = line.find_first_of(" =;([{.,+-*/?%<>&^|~", pos, i + 1);

                        variables.push_back(QString::fromStdString(line.substr(start, end)));

                        break;
                    }
                }

                s = i + 1;
            }
            else if (line[i] == '{' || line[i] == '}')
            {
                scopeLevel += (line[i] == '{') ? 1 : -1;
                s = i + 1;
            }
        }
    }

    return variables;
}

void ScriptSystem::cacheGlobalVariables(ScriptComponent &comp)
{

    if (comp.engine)
    {
        QJSValueIterator jsIt{comp.engine->globalObject()};
        while (jsIt.hasNext())
        {
            jsIt.next();
            if (!jsIt.value().property("ComponentType").isUndefined() &&
                jsIt.value().property("ComponentType").isNumber() &&
                !jsIt.value().isCallable())
            {
                globalVariables[comp.entityId].push_back(jsIt.name());
            }
        }
    }
}

void ScriptSystem::initGarbageCollection()
{

    /* Remember to clear variablecache.
     * Would'nt be a good garbage collector if it
     * didn't clear it's own garbage would it?
     */
    if (!globalVariables.empty())
        globalVariables.clear();
}

bool ScriptSystem::load(ScriptComponent& comp, const std::string& file)
{

    currentComp = &comp;

    if(!comp.JSEntity)
    {
        initializeJSEntity(comp);
    }

    if(!file.size())
    {
        return false;
    }

    QFileInfo info(QString::fromStdString(gsl::scriptsFilePath + file));

    QFile scriptFile(info.filePath());
    if(!scriptFile.exists())
    {
        qDebug() << "Script file (" + info.filePath() + ") does not exist!";
        return false;
    }
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open script: " << info.filePath();
        return false;
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    contents.prepend(mHelperFuncs);
    scriptFile.close();
    auto value = comp.engine->evaluate(contents, info.filePath());
    if(value.isError())
    {
        checkError(value);
        return false;
    }

    comp.filePath = info.baseName().toStdString() + ".js";
    currentComp = nullptr;
    currentFileName = "";
    return true;
}

void ScriptSystem::call(ScriptComponent& comp, const std::string& function)
{

    if(!comp.filePath.size() || !comp.beginplayRun)
        return;

    currentComp = &comp;
    currentFileName = QString::fromStdString(comp.filePath);

    if(!comp.JSEntity)
    {
        initializeJSEntity(comp);
    }

    QJSValue value = comp.engine->evaluate(QString::fromStdString(function), QString::fromStdString(gsl::scriptsFilePath) + currentFileName);
    if(value.isError())
    {
        checkError(value);
        return;
    }

    value = value.call();
    if(value.isError())
    {
        checkError(value);
        return;
    }

    currentComp = nullptr;
    currentFileName = "";
    return;
}

QJSValue ScriptSystem::call(ScriptComponent &comp, const std::string& function, QJSValueList params)
{

    if(!comp.filePath.size() || !comp.beginplayRun)
        return QJSValue{};

    currentComp = &comp;
    currentFileName = QString::fromStdString(comp.filePath);

    if(!comp.JSEntity)
    {
        initializeJSEntity(comp);
    }

    QJSValue value = comp.engine->evaluate(QString::fromStdString(function), QString::fromStdString(gsl::scriptsFilePath) + currentFileName);
    if(value.isError())
    {
        checkError(value);
        return value;
    }

    value = value.call(params);
    if(value.isError())
    {
        checkError(value);
        return value;
    }

    currentComp = nullptr;
    currentFileName = "";
    return value;
}

QJSValue ScriptSystem::call(const std::string& function)
{

    if(currentComp && currentComp->JSEntity)
    {
        QJSValue value = currentComp->engine->evaluate(QString::fromStdString(function), QString::fromStdString(gsl::scriptsFilePath) + currentFileName);
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
        QJSValue value = currentComp->engine->evaluate(QString::fromStdString(function), QString::fromStdString(gsl::scriptsFilePath) + currentFileName);
        if(value.isError())
        {
            checkError(value);
            return false;
        }

        auto returnValue = value.call(params);
        if(value.isError())
        {
            checkError(value);
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
        initializeJSEntity(comp);
    }

    QJSValue value = comp.engine->evaluate(contents, QString::fromStdString(gsl::scriptsFilePath) + fileName);
    if(value.isError())
    {
        checkError(value);
        return false;
    }

    value = comp.engine->evaluate(function);
    if(value.isError())
    {
        checkError(value);
        return false;
    }

    value = value.call();
    if(value.isError())
    {
        checkError(value);
        return false;
    }

    comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
    currentComp = nullptr;
    currentFileName = "";
    return true;
}

QObject* ScriptSystem::spawnCube()
{

    auto entity =  World::getWorld().getEntityManager()->createCube();
    return getEntityWrapper(entity);
}

QObject *ScriptSystem::spawnEntity()
{

    return getEntityWrapper(World::getWorld().getEntityManager()->createEntity());
}

QJSValue ScriptSystem::getAllEntityIDsByComponent(const QString& name)
{

    if(!currentComp)
        return 0;

    std::vector<int> IDs;

    auto entityManager = World::getWorld().getEntityManager();

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

    auto entityManager = World::getWorld().getEntityManager();

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

void ScriptSystem::destroyEntity(unsigned entity)
{

    World::getWorld().getEntityManager()->removeEntityLater(entity);
}

void ScriptSystem::updateJSComponent(ScriptComponent& comp)
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

                auto entityManager = World::getWorld().getEntityManager();

                // Get component that matches the type
                auto ID = static_cast<unsigned>(object["ID"].toInt());
                auto componentType = static_cast<ComponentType>(object["ComponentType"].toInt());
                auto component = entityManager->getComponent(ID, componentType);

                if(component)
                {
                    object.remove("ID");

                    // If they are different this component was modified in C++
                    // and we need to update the JS version
                    auto newJson = component->toJSON();
                    if(object != newJson)
                    {
                        auto updatedComp = comp.engine->toScriptValue(newJson);

                        for (auto key : newJson.keys())
                            comp.engine->globalObject().property("accessedComponents").property(i).setProperty(key, updatedComp.property(key));
                    }
                }
            }
        }
    }
}

void ScriptSystem::updateCPPComponent(ScriptComponent &comp)
{

    if(!comp.valid || !comp.filePath.size() || !comp.beginplayRun)
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

                auto entityManager = World::getWorld().getEntityManager();

                // Get component that matches the type
                auto ID = static_cast<unsigned>(object["ID"].toInt());
                auto componentType = static_cast<ComponentType>(object["ComponentType"].toInt());
                auto component = entityManager->getComponent(ID, componentType);

                object.remove("ID");

                if(component)
                {
                    // If they are different this component was modified in JS
                    // and we need to update the C++ version
                    auto oldJson = component->toJSON();
                    if(object != oldJson)
                    {
                        component->fromJSON(object);
                    }
                }
                else
                {
                    // If the component does not exist it needs to be added. Deferred spawning.
                    for(auto& info : entityManager->getEntityInfos())
                    {
                        if(ID == info.entityId)
                        {
                            auto comp = entityManager->addComponent(ID, componentType);
                            comp->fromJSON(object);
                            comp->valid = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ScriptSystem::updateCPPComponent(ScriptComponent &comp, QJSValue compList)
{
    PROFILE_FUNCTION();
    if(!comp.valid || !comp.filePath.size() || !comp.beginplayRun)
        return;

    if(!compList.isUndefined() && !compList.isNull())
    {
        auto length = compList.property("length").toInt();
        if(length > 0)
        {
            for(unsigned i = 0; i < static_cast<unsigned>(length); ++i)
            {
                auto object = QJsonValue::fromVariant(compList.property(i).toVariant()).toObject();

                if(object.isEmpty())
                    continue;

                auto entityManager = World::getWorld().getEntityManager();

                // Get component that matches the type
                auto ID = static_cast<unsigned>(object["ID"].toInt());
                auto componentType = static_cast<ComponentType>(object["ComponentType"].toInt());
                auto component = entityManager->getComponent(ID, componentType);

                object.remove("ID");

                if(component)
                {
                    // If they are different this component was modified in JS
                    // and we need to update the C++ version
                    auto oldJson = component->toJSON();
                    if(object != oldJson)
                    {
                        component->fromJSON(object);
                    }
                }
                else
                {
                    // If the component does not exist it needs to be added. Deferred spawning.
                    for(auto& info : entityManager->getEntityInfos())
                    {
                        if(ID == info.entityId)
                        {
                            auto comp = entityManager->addComponent(ID, componentType);
                            comp->fromJSON(object);
                            comp->valid = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ScriptSystem::initializeJSEntity(ScriptComponent& comp)
{

    comp.JSEntity = ScriptSystem::get()->getEntityWrapper(comp.entityId);
    comp.engine->globalObject().setProperty("me", comp.engine->newQObject(comp.JSEntity));
    comp.engine->globalObject().setProperty("accessedComponents", comp.engine->newArray());
}

void ScriptSystem::initializeHelperFuncs()
{

    QFile file(QString::fromStdString("../INNgine2019/JSHelperFuncs.js"));
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR Script System: Failed to find JSHelperFuncs.js!";
        return;
    }

    mHelperFuncs = file.readAll();
    file.close();

    file.setFileName("../INNgine2019/JSMath.js");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR Script System: Failed to find JSMath.js!";
        return;
    }

    mHelperFuncs.prepend(file.readAll());
    file.close();
}
