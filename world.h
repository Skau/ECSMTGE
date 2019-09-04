#ifndef WORLD_H
#define WORLD_H

#include <QObject>

class Scene;
class EntityManager;

class World : public QObject
{
    Q_OBJECT

public:
    World();

    Scene* getCurrentScene() { return mCurrentScene; }

    EntityManager* getEntityManager() { return entityManager; }

private:
    std::vector<Scene*> mScenes;

    Scene* mCurrentScene;

    EntityManager* entityManager;
};

#endif // WORLD_H
