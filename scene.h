#ifndef SCENE_H
#define SCENE_H

#include <QObject>

class VisualObject;

class Scene : public QObject, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    Scene();

    const std::vector<VisualObject*>& getVisualObjects() { return mVisualObjects; }

private:
    std::vector<VisualObject*> mVisualObjects;
};

#endif // SCENE_H
