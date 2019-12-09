#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <memory>
#include "scene.h"

class EntityManager;
class CameraComponent;

/** The world class is a singleton class managing objects in the game instance.
 * This includes the EntityManager and all scenes.
 * @brief The world class is a singleton class managing objects in the game instance.
 * This includes the EntityManager and all scenes.
 */
class World : public QObject
{
    Q_OBJECT
    friend class App;

private:
    World();

    static World* mWorldInstance;

public:
    World(const World& wld) = delete;
    World& operator= (const World& wld) = delete;

    static World& getWorld();

    std::shared_ptr<EntityManager> getEntityManager() { return entityManager; }

    /**
     * @brief Returns the appropriate camera based on the value given. Either the editor or game camera.
     */
    CameraComponent* getCurrentCamera(bool currentlyPlaying);

    /**
     * @brief Creates a new blank scene with minimal setup.
     */
    void initBlankScene();

    /**
     * @brief Removes all entities in the current scene.
     */
    void clearEntities();

    /**
     * @brief Called right after the game is stopped.
     */
    void loadTemp();

    /**
     * @brief Called right before the game is started.
     */
    void saveTemp();

    /**
     * @brief Returns a bool based on if a current scene is active.
     */
    bool isSceneValid() const;

    std::optional<std::string> sceneFilePath() const;

    void saveScene(const std::string& path);

    void loadScene(const std::string& path);

    void newScene();

    ~World();

signals:
    void updateSceneName(const std::string& name);

private:
    std::unique_ptr<Scene> mCurrentScene;
    // Need to save a copy of the filename for the scene because
    // the sceens gets changed out with a temporary scene whenever
    // we go into play mode.
    std::optional<std::string> sceneFileName;

    std::shared_ptr<EntityManager> entityManager;
};

#endif // WORLD_H
