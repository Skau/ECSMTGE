#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include <QObject>
#include <QJSEngine>
#include <memory>
#include <vector>

class EntityManager;
class QJSEngine;
class ScriptComponent;
class QEntity;

/**
 * The instance of this class is given to all engines (all script components have one each)
 * as a global object under the name engine.
 * This means that any public slots or Q_INVOKABLE public functions in this class are callable from JS.
 * Example JS code: var entity = engine.spawnCube(2, 0, 5);
 */
class ScriptSystem : public QObject
{
    Q_OBJECT
public:
    static ScriptSystem* get()
    {
        static ScriptSystem instance;
        return &instance;
    }

    void beginPlay(std::vector<ScriptComponent>& comps);
    void tick(float deltaTime, std::vector<ScriptComponent>& comps);
    void endPlay(std::vector<ScriptComponent>& comps);

    // EntityManager given by App
    void setEntityManager(std::shared_ptr<EntityManager> entityManager){ this->entityManager = entityManager; }

    static QString checkError(QJSValue value);

    QEntity* getEntityWrapper(unsigned int entity);

public slots:
    QObject* spawnCube(float x, float y, float z);

private:
    ScriptSystem(){}

    std::shared_ptr<EntityManager> entityManager;
};

#endif // SCRIPTSYSTEM_H
