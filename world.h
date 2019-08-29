#ifndef WORLD_H
#define WORLD_H

#include <QObject>

class Scene;

class World : public QObject
{
    Q_OBJECT

public:
    World();

    Scene* getCurrentScene() { return mCurrentScene; }

private:
    std::vector<Scene*> mScenes;

    Scene* mCurrentScene;
};

#endif // WORLD_H
