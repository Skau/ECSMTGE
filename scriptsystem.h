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
class HitInfo;

/**
 * The instance of this class is given to all engines (all script components have one each)
 * as a global object under the name engine.
 * This means that any public slots or Q_INVOKABLE public functions in this class are callable from JS.
 * Example JS code: let entity = engine.spawnCube();
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
    void runKeyEvent(ScriptComponent &comp, const QString& key);
    void runHitEvents(std::vector<ScriptComponent>& comps, std::vector<HitInfo> hitInfos);

    // EntityManager given by App
    void setEntityManager(std::shared_ptr<EntityManager> entityManager){ this->entityManager = entityManager; }

    static QString checkError(QJSValue value);

    QEntity* getEntityWrapper(unsigned int entity);

    // Hardcoded functions provided in all scripts
    const QString getEntityHelperFunctions() { return QString(addCompFunc + getCompFunc); }

    /**
     * @brief Loads the js file given. Returns true if the file is successfully evaluated and set.
     */
    bool load(ScriptComponent& comp, const std::string& file);

    /**
     * @brief Calls a JS function on the current file loaded. Returns true if successful.
     */
    void call(ScriptComponent& comp, const std::string& function);

    /**
     * @brief Calls a JS function on the current file loaded. Returns true if successful.
     */
    QJSValue call(const std::string& function);

    /**
     * @brief Calls a JS function with params on the current file loaded. Returns true if successful.
     */
    void call(ScriptComponent& comp, const std::string& function, QJSValueList params);


    /**
     * @brief Calls a JS function with params on the current file loaded. Returns true if successful.
     */
    QJSValue call(const std::string& function, QJSValueList params);

    /**
     * @brief Executes one off raw js code from the mini editor. Returns true if successfull.
     */
    bool execute(ScriptComponent& comp, QString function, QString contents, QString fileName);


public slots:
    /**
     * @brief Spawns and returns a cube at (0,0,0). This includes a transform- and mesh component. Mesh is set to a cube. Defaults to visible.
     * Example: let cube = engine.spawnCube();
     */
    QObject* spawnCube();
    /**
     * @brief Spawns and returns an entity with no components.
     * Example: let spawnedEntity = engine.spawnEntity();
     */
    QObject* spawnEntity();

    /**
     * @brief Returns an array of all entity IDs that has the given component.
     * Example: let entityIDs = engine.getAllEntityIDsByComponent("mesh");
     */
    QJSValue getAllEntityIDsByComponent(const QString& name);

    /**
     * @brief Returns an array of all entityIDs.
     * Example: let entityIDs = engine.getAllEntityIDs();
     */
    QJSValue getAllEntityIDs();

    /**
     * @brief Returns an entity with the given ID.
     * Example: let entity = getEntity(2);
     */
    QObject* getEntity(unsigned int id);

private:
    ScriptSystem(){}

    std::shared_ptr<EntityManager> entityManager;

    void updateComponents();
    void updateJSComponents(ScriptComponent& comp);

    // Cached
    ScriptComponent* currentComp;
    QString currentFileName{};


    const QString addCompFunc = "function addComponent(name, id = 0)"
                                "{\n"
                                "let comp = me._addComponent(name, id);\n"
                                "if(comp != null)\n"
                                "{\n"
                                "accessedComponents.push(comp);\n"
                                "}\n"
                                "return comp;\n"
                                "}\n";

    const QString getCompFunc = "function getComponent(name, id = 0)"
                                "{\n"
                                "let comp = me._getComponent(name, id);\n"
                                "if(comp != null)\n"
                                "{\n"
                                "accessedComponents.push(comp);\n"
                                "}\n"
                                "return comp;\n"
                                "}\n";
};

#endif // SCRIPTSYSTEM_H
