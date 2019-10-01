#include "world.h"
#include "resourcemanager.h"
#include "entitymanager.h"
#include "scene.h"


World::World()
{
    ResourceManager::instance()->addShader("color",             std::make_shared<Shader>("colorshader"));
    ResourceManager::instance()->addShader("texture",           std::make_shared<Shader>("textureshader"));
    ResourceManager::instance()->addShader("phong",             std::make_shared<Shader>("phongshader"));
    ResourceManager::instance()->addShader("axis",              std::make_shared<Shader>("axisshader.vert", "colorshader.frag"));
    ResourceManager::instance()->addShader("defaultDeferred",   std::make_shared<Shader>("/Deferred/gBuffer.vert", "/Deferred/gBuffer.frag"));
    ResourceManager::instance()->addShader("directionalLight",  std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/directionallight.frag"));
    ResourceManager::instance()->addShader("pointLight",        std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/pointlight.frag"));
    ResourceManager::instance()->addShader("spotLight",         std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/spotlight.frag"));
    ResourceManager::instance()->addShader("skybox",            std::make_shared<Shader>("skybox.vert", "skybox.frag"));

    // This function is troublesome...
    // ResourceManager::instance()->LoadAssetFiles();

    ResourceManager::instance()->addMesh("skybox", "skybox.txt");
    ResourceManager::instance()->addMesh("box2", "box2.txt");
    ResourceManager::instance()->addMesh("axis", "axis.txt");
    ResourceManager::instance()->addMesh("suzanne", "monkey.obj");

    ResourceManager::instance()->loadWav("Caravan_mono", std::string{gsl::soundsFilePath}.append("Caravan_mono.wav"));
    ResourceManager::instance()->loadWav("explosion", std::string{gsl::soundsFilePath}.append("explosion.wav"));
    ResourceManager::instance()->loadWav("laser", std::string{gsl::soundsFilePath}.append("laser.wav"));
    ResourceManager::instance()->loadWav("stereo", std::string{gsl::soundsFilePath}.append("stereo.wav"));

    ResourceManager::instance()->getMesh("axis")->mRenderType = GL_LINES;
    ResourceManager::instance()->getMesh("axis")->mMaterial.mShader = ResourceManager::instance()->getShader("axis");

    ResourceManager::instance()->getMesh("skybox")->mMaterial.mShader = ResourceManager::instance()->getShader("skybox");
    ResourceManager::instance()->addCubemapTexture("skybox", "skyboxSpaceBoring.bmp");
    ResourceManager::instance()->getMesh("skybox")->mMaterial.mTexture = ResourceManager::instance()->getTexture("skybox");

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
