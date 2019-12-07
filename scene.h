#ifndef SCENE_H
#define SCENE_H

#include "resourcemanager.h"
#include "entitymanager.h"
#include <optional>

class World;

/**
 * @brief A helper class for maintaing what scene we're in. Note that this class actually doesn't hold onto any data, it just helps loading/saving/clearing the current entities in the scene.
 */
class Scene : public QObject
{
    Q_OBJECT
public:
    Scene(World* world);
    virtual ~Scene();

    std::string name = "Scene";
    std::optional<std::string> filePath{};

    /**
     * @brief Creates an empty scene with only a camera
     */
    void initBlankScene();

    /**
     * @brief Derive from this class and override this to add entities on startup via C++
     */
    virtual void initCustomObjects(){}

    bool LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path);

signals:
    void loadingFromFile();

protected:
    World* mWorld;
};

/** Temp class to demonstrate how a scene with custom logic could be implemented.
 * @brief Temp class to demonstrate how a scene with custom logic could be implemented.
 */
class TestScene : public Scene
{
public:
    TestScene(World* world);
    void initCustomObjects() override;
};


#endif // SCENE_H
