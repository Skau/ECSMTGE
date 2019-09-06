#ifndef SCENE_H
#define SCENE_H

#include "resourcemanager.h"
#include "world.h"
#include "entitymanager.h"

class Scene : public QObject
{
    Q_OBJECT
public:
    Scene();
    virtual ~Scene();
    virtual void initObjects(World* mWorld)=0;

    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path);
};

class TestScene : public Scene
{
public:
    void initObjects(World* mWorld) override
    {
        auto entityManager = mWorld->getEntityManager();
        for(int i = 0; i < 10; ++i)
        {
            auto entity = entityManager->createEntity();
            entityManager->addComponent<Transform, Render>(entity);
            auto render = entityManager->getComponent<Render>(entity);
            if(auto meshData = ResourceManager::instance()->getMesh("box"))
            {
                render->meshData = *meshData;
                render->isVisible = true;
            }
            auto transform = entityManager->getComponent<Transform>(entity);
            transform->position = gsl::vec3(i*2, 0, 0);
        }
    }
};


#endif // SCENE_H
