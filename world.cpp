#include "world.h"
#include "resourcemanager.h"
#include "entitymanager.h"
#include "scene.h"


World::World()
{
    ResourceManager::instance()->addShader("color",     std::make_shared<Shader>("colorshader"));
    ResourceManager::instance()->addShader("texture",   std::make_shared<Shader>("textureshader"));
    ResourceManager::instance()->addShader("phong",     std::make_shared<Shader>("phongshader"));
    ResourceManager::instance()->addShader("axis",      std::make_shared<Shader>("axisshader.vert", "colorshader.frag"));

    ResourceManager::instance()->LoadAssetFiles();

    ResourceManager::instance()->getMesh("axis")->mRenderType = GL_LINES;
    ResourceManager::instance()->getMesh("axis")->mMaterial.mShader = ResourceManager::instance()->getShader("axis");

    entityManager = std::make_shared<EntityManager>();

    mCurrentScene = new TestScene();
    //mCurrentScene = new EmptyScene();
}

World::~World()
{
}

void World::initCurrentScene()
{
    mCurrentScene->initObjects(this);
}
