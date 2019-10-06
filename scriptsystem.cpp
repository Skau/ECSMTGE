#include "scriptsystem.h"
#include <QFile>
#include <QTextStream>

ScriptSystem::ScriptSystem()
{
    mJSEngine.installExtensions(QJSEngine::ConsoleExtension);
    mGlobalObject = mJSEngine.globalObject();
    mGlobalObject.setProperty("engine", mJSEngine.newQObject(this));
}


bool ScriptSystem::load(ScriptComponent comp)
{
    QString fileName = QString::fromStdString(comp.filePath);
    if(!fileName.size())
    {
        return false;
    }
    QFile scriptFile(fileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open script: " << fileName;
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    auto value = mJSEngine.evaluate(contents, fileName);
    if(value.isError())
    {
        checkError(value);
        return false;
    }
    return true;
}


bool ScriptSystem::evaluate(const std::string& program, std::string& errorMessage, QJSValue& jsValue)
{
    jsValue = mJSEngine.evaluate(QString::fromStdString(program));
    if(jsValue.isError())
    {
        errorMessage = checkError(jsValue).toStdString();
        return false;
    }
    return true;
}

void ScriptSystem::beginPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(!load(comp))
        {
            continue;
        }

        QJSValue value = mJSEngine.evaluate("beginPlay");
        if(value.isError())
        {
            checkError(value);
            return;
        }

        mGlobalObject.setProperty("entity", comp.entityId);
        value.call();
    }
}


void ScriptSystem::tick(float deltaTime, std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(!load(comp))
        {
            continue;
        }

        QJSValue value = mJSEngine.evaluate("tick");
        if(value.isError())
        {
            checkError(value);
            continue;
        }

        mGlobalObject.setProperty("entity", comp.entityId);
        QJSValueList list;
        list << deltaTime;
        value.call(list);
    }
}

void ScriptSystem::endPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(!load(comp))
        {
            continue;
        }

        QJSValue value = mJSEngine.evaluate("endPlay");
        if(value.isError())
        {
            checkError(value);
            continue;
        }

        mGlobalObject.setProperty("entity", comp.entityId);
        value.call();
    }
}

void ScriptSystem::call(QString function, QString contents, QString fileName, ScriptComponent& comp)
{
    QJSValue value = mJSEngine.evaluate(contents, fileName);
    if(value.isError())
    {
        checkError(value);
        return;
    }

    value = mJSEngine.evaluate(function);
    if(value.isError())
    {
        checkError(value);
        return;
    }

    mGlobalObject.setProperty("entity", comp.entityId);
    value.call();
}

void ScriptSystem::call(const std::string& function, QJSValueList params, ScriptComponent& comp)
{
    if(!load(comp))
    {
        return;
    }

    QJSValue value = mJSEngine.evaluate(QString::fromStdString(function));
    if(value.isError())
    {
        checkError(value);
        return;
    }

    mGlobalObject.setProperty("entity", comp.entityId);
    value.call(params);
}

void ScriptSystem::call(const std::string& function, QJSValueList params, std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(!load(comp))
        {
            continue;
        }

        QJSValue value = mJSEngine.evaluate(QString::fromStdString(function));
        if(value.isError())
        {
            checkError(value);
            continue;
        }

        mGlobalObject.setProperty("entity", comp.entityId);
        value.call(params);
    }
}


void ScriptSystem::call(const std::string& function, std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        if(!load(comp))
        {
            continue;
        }

        QJSValue value = mJSEngine.evaluate(QString::fromStdString(function));
        if(value.isError())
        {
            checkError(value);
            continue;
        }

        mGlobalObject.setProperty("entity", comp.entityId);
        value.call();
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
