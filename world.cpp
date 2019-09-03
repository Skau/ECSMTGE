#include "world.h"

#include "resourcemanager.h"
#include "scene.h"

World::World()
{
    ResourceManager::instance()->addShader("plain", new ColorShader("plainshader"));
    ResourceManager::instance()->addShader("texture", new ColorShader("textureshader"));
    ResourceManager::instance()->addShader("phong", new ColorShader("phongshader"));

    ResourceManager::instance()->loadTexture("white", "white.bmp");
    ResourceManager::instance()->loadTexture("hund", "hund.bmp");
    ResourceManager::instance()->loadTexture("skybox", "skybox.bmp");

    mCurrentScene = new Scene();
}
