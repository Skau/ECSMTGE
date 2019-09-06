#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <memory>

class Scene;
class EntityManager;

class World : public QObject
{
    Q_OBJECT

public:
    World();

    Scene* getCurrentScene() { return mCurrentScene; }

    std::shared_ptr<EntityManager> getEntityManager() { return entityManager; }

    ~World();

    void initCurrentScene();

private:
    std::vector<Scene*> mScenes;

    Scene* mCurrentScene;

    std::shared_ptr<EntityManager> entityManager;
};

#endif // WORLD_H
