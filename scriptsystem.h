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

    QJSEngine* engine;

    /**
     * @brief Loads the js file given. Returns true if the file is successfully evaluated and set.
     */
    bool load(ScriptComponent& comp, const std::string& file);

    /**
     * @brief Calls a JS function on the current file loaded. Returns true if successful.
     */
    bool call(ScriptComponent& comp, const std::string& function);

    /**
     * @brief Calls a JS function with params on the current file loaded. Returns true if successful.
     */
    bool call(ScriptComponent& comp, const std::string& function, QJSValueList params);

    /**
     * @brief Executes one off raw js code from the mini editor. Returns true if successfull.
     */
    bool execute(ScriptComponent& comp, QString function, QString contents, QString fileName);

    QEntity* currentQEntity;

    // Hardcoded functions proved in all scripts
    const QString addCompFunc = "function addComponent(name)"
                                "{"
                                "var comp = entity.addComponent(name);"
                                "if(comp)"
                                "{"
                                "accessedComponents.push(comp);"
                                "}"
                                "return comp}";

    const QString getCompFunc = "function getComponent(name)"
                                "{"
                                "var comp = entity.getComponent(name);"
                                "if(comp)"
                                "{"
                                "accessedComponents.push(comp);"
                                "}"
                                "return comp}";

public slots:
    QObject* spawnCube(float x, float y, float z);
    QObject* spawnEntity();

private:
    ScriptSystem();

    std::shared_ptr<EntityManager> entityManager;

    /**
     * @brief Gets components accessed during the JS script and checks if anything was modified
     */
    void checkForModifiedComponents(ScriptComponent &comp);
};

#endif // SCRIPTSYSTEM_H
