#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <memory>
#include "scene.h"

class EntityManager;

class World : public QObject
{
    Q_OBJECT

public:
    World();
    ~World();

    std::shared_ptr<EntityManager> getEntityManager() { return entityManager; }

    void initBlankScene();

    void clearEntities();

    void loadTemp();

    void saveTemp();

    bool isSceneValid() const;

public slots:
    void saveScene(const std::string& path);
    void loadScene(const std::string& path);
    void newScene();

signals:
    void sceneLoaded();

private:
    std::unique_ptr<Scene> mCurrentScene;

    std::shared_ptr<EntityManager> entityManager;
};

#endif // WORLD_H
