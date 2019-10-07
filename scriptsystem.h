#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include <QObject>
#include <QJSEngine>
#include <memory>
#include <vector>
#include "componentdata.h"

class EntityManager;
class QJSEngine;

class ScriptSystem : public QObject
{
    Q_OBJECT
public:
    static ScriptSystem* get()
    {
        static ScriptSystem instance;
        return &instance;
    }

    bool evaluate(const std::string& text, std::string& errorMessage, QJSValue& jsValue);

    void beginPlay(std::vector<ScriptComponent>& comps);
    void tick(float deltaTime, std::vector<ScriptComponent>& comps);
    void endPlay(std::vector<ScriptComponent>& comps);
    void call(QString function, QString contents, QString fileName, ScriptComponent& comp);
    void setEntityManager(std::shared_ptr<EntityManager> entityManager){ this->entityManager = entityManager; }

public slots:
    // This is callable from JS (e.g. engine.setPosition(entity, 0, 0, 0);)
    void setPosition(unsigned int entity, float x, float y, float z);

private:
    ScriptSystem();
    QString checkError(QJSValue value);
    bool load(ScriptComponent comp);

    QJSEngine mJSEngine;
    QJSValue mGlobalObject;
    std::shared_ptr<EntityManager> entityManager;
};

#endif // SCRIPTSYSTEM_H
