#include "scriptsystem.h"
#include <QFile>
#include <QTextStream>
#include "entitymanager.h"
#include "componentdata.h"

void ScriptSystem::beginPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        comp.call("beginPlay");
    }
}

void ScriptSystem::tick(float deltaTime, std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        QJSValueList list;
        list << deltaTime;
        comp.call("tick", list);
    }
}

void ScriptSystem::endPlay(std::vector<ScriptComponent>& comps)
{
    for(auto& comp : comps)
    {
        comp.call("endPlay");
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


void ScriptSystem::setPosition(unsigned int entity, float x, float y, float z)
{
    if(!entityManager)
        return;

    entityManager->setTransformPos(entity, gsl::vec3(x, y, z));
}



