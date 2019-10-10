#include "scene.h"
#include "world.h"

Scene::Scene(World* world)
    : mWorld(world)
{
    initBlankScene();
}

Scene::Scene(World* world, const std::string& path)
    : mWorld(world)
{
    path.size() != 0 ? LoadFromFile(path) : initBlankScene();
}

Scene::~Scene()
{
    mWorld = nullptr;
}

void Scene::initBlankScene()
{
    auto entityManager = mWorld->getEntityManager();

    // Camera:
    auto camera = entityManager->createEntity("mainCam");
    auto [camTrans, camCam, dirLight, camInput] = entityManager->addComponent<TransformComponent, CameraComponent, DirectionalLightComponent, InputComponent>(camera);
    camTrans.setPosition(gsl::vec3{0.f, 0.f, 5.f});
    camInput.isCurrentlyControlled = true;
    camCam.isCurrentActive = true;
}

void Scene::LoadFromFile(const std::string& path)
{

}

void Scene::SaveToFile(const std::string& path)
{

}


// *** Derived scenes *** //

TestScene::TestScene(World* world)
    : Scene(world)
{
}

void TestScene::initCustomObjects()
{
    auto entityManager = mWorld->getEntityManager();
    for(int i = 0; i < 10; ++i)
    {
        auto entity = entityManager->createEntity();
        auto [transform, render] = entityManager->addComponent<TransformComponent, MeshComponent>(entity);
        if(auto meshData = ResourceManager::instance()->getMesh("suzanne"))
        {
            render.meshData = *meshData;
            render.isVisible = true;
        }
        render.mMaterial.mParameters =
        {
            {"color", gsl::vec3(1.f, 0, 0)}
        };
        transform.setPosition(gsl::vec3(i*2.f, 0, 0));
    }
}
