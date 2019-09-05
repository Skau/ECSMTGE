#include "world.h"

#include "resourcemanager.h"
#include "entitymanager.h"
#include "scene.h"


World::World()
{
    ResourceManager::instance()->addShader("plain",     std::make_shared<ColorShader>("plainshader"));
    ResourceManager::instance()->addShader("texture",   std::make_shared<ColorShader>("textureshader"));
    ResourceManager::instance()->addShader("phong",     std::make_shared<ColorShader>("phongshader"));

    ResourceManager::instance()->loadTexture("white",   "white.bmp");
    ResourceManager::instance()->loadTexture("hund",    "hund.bmp");
    ResourceManager::instance()->loadTexture("skybox",  "skybox.bmp");

    ResourceManager::instance()->addMesh("box", "box2.txt");
    ResourceManager::instance()->addMesh("monkey", "monkey.obj");

    entityManager = new EntityManager();

    mCurrentScene = new Scene(this);

}
