#ifndef SCENE_H
#define SCENE_H

#include "resourcemanager.h"
#include "entitymanager.h"

class World;

class Scene : public QObject
{
    Q_OBJECT
public:
    Scene(World* world);
    Scene(World* world, const std::string& path);
    virtual ~Scene();

    /**
     * @brief Creates an empty scene with only a camera
     */
    void initBlankScene();

    /**
     * @brief Derive from this class and override this to add entities on startup via C++
     */
    virtual void initCustomObjects(){}

    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path);

protected:
    World* mWorld;
};

class TestScene : public Scene
{
public:
    TestScene(World* world);
    void initCustomObjects() override;
};


#endif // SCENE_H
