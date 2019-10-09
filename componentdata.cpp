#include "componentdata.h"
#include <stack>


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

bool ScriptComponent::load(const std::string& file)
{
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
