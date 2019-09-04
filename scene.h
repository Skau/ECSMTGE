#ifndef SCENE_H
#define SCENE_H

#include <QObject>

class VisualObject;
class World;

class Scene : public QObject, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    Scene(World* world);

    const std::vector<VisualObject*>& getVisualObjects() { return mVisualObjects; }

private:
    std::vector<VisualObject*> mVisualObjects;
    World* mWorld;
};

#endif // SCENE_H
