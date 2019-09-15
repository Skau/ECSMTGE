#ifndef SCENE_H
#define SCENE_H

#include "resourcemanager.h"
#include "world.h"
#include "entitymanager.h"
#include "soundsource.h"

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

class EmptyScene : public Scene
{
public:
    void initObjects(World* mWorld) override
    {
        auto entityManager = mWorld->getEntityManager();
        // Camera:
        auto camera = entityManager->createEntity("mainCam");
        auto [trans, cam] = entityManager->addComponent<TransformComponent, CameraComponent>(camera);
        trans.position = gsl::vec3{0.f, 0.f, 5.f};
        trans.rotation = gsl::quat{0.f, 0.f, 90.f, 0.f};
        trans.updated = true;
    }
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
            auto [transform, render] = entityManager->addComponent<TransformComponent, MeshComponent>(entity);
            if(auto meshData = ResourceManager::instance()->getMesh("box2"))
            {
                render.meshData = *meshData;
                render.isVisible = true;
            }
            transform.position = gsl::vec3(i*2, 0, 0);
        }

        // Camera:
        auto camera = entityManager->createEntity("mainCam");
        auto [trans, cam] = entityManager->addComponent<TransformComponent, CameraComponent>(camera);
        trans.position = gsl::vec3{0.f, 0.f, 5.f};
        trans.rotation = gsl::quat{0.f, 0.f, 90.f, 0.f};
        trans.updated = true;

        // Axis:
        auto [axisTrans, axisMesh] = entityManager->addComponent<TransformComponent, MeshComponent>(entityManager->createEntity("axis"));
        axisMesh.meshData = *ResourceManager::instance()->getMesh("axis");
        axisMesh.isVisible = true;

        auto laserSound = ResourceManager::instance()->createSource("laser", true);
        laserSound->play();
    }
};


#endif // SCENE_H
