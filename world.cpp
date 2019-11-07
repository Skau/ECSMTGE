#include "world.h"
#include "resourcemanager.h"
#include "entitymanager.h"

World::World()
{
    // Forward
    ResourceManager::instance()->addShader("color",             std::make_shared<Shader>("colorshader", ShaderType::Forward));
    ResourceManager::instance()->addShader("texture",           std::make_shared<Shader>("textureshader", ShaderType::Forward));
    ResourceManager::instance()->addShader("phong",             std::make_shared<Shader>("phongshader", ShaderType::Forward));
    ResourceManager::instance()->addShader("skybox",            std::make_shared<Shader>("skybox", ShaderType::Forward));
    ResourceManager::instance()->addShader("white",             std::make_shared<Shader>("white", ShaderType::Forward));
    ResourceManager::instance()->addShader("axis",              std::make_shared<Shader>("axisshader.vert", "colorshader.frag", ShaderType::Forward));
    ResourceManager::instance()->addShader("singleColor",       std::make_shared<Shader>("white.vert", "singleColor.frag", ShaderType::Forward));

    // Deferred
    ResourceManager::instance()->addShader("defaultDeferred",   std::make_shared<Shader>("/Deferred/gBuffer.vert", "/Deferred/gBuffer.frag", ShaderType::Deferred));
    ResourceManager::instance()->addShader("directionalLight",  std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/directionallight.frag", ShaderType::Deferred));
    ResourceManager::instance()->addShader("pointLight",        std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/pointlight.frag", ShaderType::Deferred));
    ResourceManager::instance()->addShader("spotLight",         std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/spotlight.frag", ShaderType::Deferred));

    // Post prosessing
    ResourceManager::instance()->addShader("passthrough",       std::make_shared<Shader>("pass.vert", "pass.frag", ShaderType::PostProcessing));
    ResourceManager::instance()->addShader("blur",              std::make_shared<Shader>("pass.vert", "blur.frag", ShaderType::PostProcessing));
    ResourceManager::instance()->addShader("ui_singleColor",    std::make_shared<Shader>("pass.vert", "singleColor.frag", ShaderType::PostProcessing));
    ResourceManager::instance()->addShader("blend",             std::make_shared<Shader>("pass.vert", "blend.frag", ShaderType::PostProcessing));

    // Other..
    ResourceManager::instance()->addShader("mousepicking",      std::make_shared<Shader>("mousepicking.vert", "mousepicking.frag", ShaderType::WeirdStuff));

    // This function is troublesome...
    // ResourceManager::instance()->LoadAssetFiles();

    ResourceManager::instance()->addMesh("skybox", "skybox.txt");
    ResourceManager::instance()->addMesh("box2", "box2.txt");
    ResourceManager::instance()->addMesh("axis", "axis.txt");
    ResourceManager::instance()->addMesh("suzanne", "monkey.obj");
    auto ball = ResourceManager::instance()->addMesh("ball", "octoball.txt");
    ResourceManager::instance()->setupLOD(ball, ResourceManager::instance()->addMesh("ball_l2", "octoball_L02.txt"), 2);
    ResourceManager::instance()->setupLOD(ball, ResourceManager::instance()->addMesh("ball_l1", "octoball_L01.txt"), 1);
    ResourceManager::instance()->addMesh("camera", "camera.obj");


    ResourceManager::instance()->loadWav("Caravan_mono", std::string{gsl::soundsFilePath}.append("Caravan_mono.wav"));
    ResourceManager::instance()->loadWav("explosion", std::string{gsl::soundsFilePath}.append("explosion.wav"));
    ResourceManager::instance()->loadWav("laser", std::string{gsl::soundsFilePath}.append("laser.wav"));
    ResourceManager::instance()->loadWav("stereo", std::string{gsl::soundsFilePath}.append("stereo.wav"));

    ResourceManager::instance()->addTexture("skybox", "skyboxSpaceBoring.bmp", GL_TEXTURE_CUBE_MAP);

    entityManager = std::make_shared<EntityManager>();
}

World::~World()
{
}

void World::initBlankScene()
{
    mCurrentScene = std::make_unique<Scene>(this);

    mCurrentScene->initBlankScene();
    mCurrentScene->initCustomObjects();
}

void World::saveScene(const std::string& path)
{
    if(mCurrentScene)
    {
        mCurrentScene->SaveToFile(path);
    }
}

void World::loadScene(const std::string& path)
{
    if(mCurrentScene)
    {
        // Cleanup
        if (auto obj = mCurrentScene.release())
            delete obj;
    }

    mCurrentScene = std::make_unique<Scene>(this);

    if (!mCurrentScene->LoadFromFile(path))
    {
        // Cleanup
        if (auto obj = mCurrentScene.release())
            delete obj;
    }
}

void World::newScene()
{
    entityManager->clear();
    initBlankScene();
}

void World::clearEntities()
{
    entityManager->clear();
}

void World::loadTemp()
{
    if(mCurrentScene)
    {
        auto name = mCurrentScene->name;
        mCurrentScene.reset();
        mCurrentScene = std::make_unique<Scene>(this);
        mCurrentScene->LoadFromFile("temp" + name + ".json");
    }
}

void World::saveTemp()
{
    if(mCurrentScene)
    {
        mCurrentScene->SaveToFile("temp" + mCurrentScene->name + ".json");
    }
}

bool World::isSceneValid() const
{
    return static_cast<bool>(mCurrentScene);
}

std::optional<std::string> World::sceneFilePath() const
{
    if (mCurrentScene && mCurrentScene->filePath)
        return mCurrentScene->filePath;

    return std::nullopt;
}
