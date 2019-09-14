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

    ResourceManager::instance()->addTexture("white",   "white.bmp");
    ResourceManager::instance()->addTexture("hund",    "hund.bmp");
    ResourceManager::instance()->addTexture("skybox",  "skybox.bmp");

    ResourceManager::instance()->addMesh("box2", "box2.txt");
    ResourceManager::instance()->addMesh("monkey", "monkey.obj");

    ResourceManager::instance()->loadWav("laser", "laser.wav");

    if (auto mesh =  ResourceManager::instance()->getMesh("box2"))
    {
        mesh->mMaterial.mShader = ResourceManager::instance()->getShader("plain");
    }

    if (auto mesh = ResourceManager::instance()->getMesh("monkey"))
    {
        mesh->mMaterial.mShader = ResourceManager::instance()->getShader("phong");
    }

    if (auto mesh = ResourceManager::instance()->addMesh("axis", "axis.txt", GL_LINES))
    {
       mesh->mMaterial.mShader = ResourceManager::instance()->getShader("axis");
    }

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
