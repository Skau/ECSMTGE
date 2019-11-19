#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <memory>
#include "scene.h"

class EntityManager;
class CameraComponent;

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

    CameraComponent* getCurrentCamera(bool currentlyPlaying);

    void initBlankScene();

    void clearEntities();

    void loadTemp();

    void saveTemp();

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
