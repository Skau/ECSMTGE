#include "world.h"
#include "resourcemanager.h"
#include "entitymanager.h"
#include <cassert>

World* World::mWorldInstance{nullptr};

World::World()
{
    // Set yourself to be the singleton instance
    mWorldInstance = this;

    // Forward
    ResourceManager::instance().addShader("singleColor",       std::make_shared<Shader>("white.vert", "singleColor.frag", ShaderType::Forward));

    // Deferred
    ResourceManager::instance().addShader("phong",             std::make_shared<Shader>("/Deferred/gBuffer.vert", "/Deferred/gBuffer.frag", ShaderType::Deferred));

    // Lights for deferred
    ResourceManager::instance().addShader("directionalLight",  std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/directionallight.frag", ShaderType::Light));
    ResourceManager::instance().addShader("pointLight",        std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/pointlight.frag", ShaderType::Light));
    ResourceManager::instance().addShader("spotLight",         std::make_shared<Shader>("/Deferred/light.vert", "/Deferred/spotlight.frag", ShaderType::Light));

    // Post prosessing
    ResourceManager::instance().addShader("passthrough",       std::make_shared<Shader>("pass.vert", "pass.frag", ShaderType::PostProcessing));
    ResourceManager::instance().addShader("blur",              std::make_shared<Shader>("pass.vert", "blur.frag", ShaderType::PostProcessing));
    ResourceManager::instance().addShader("ui_singleColor",    std::make_shared<Shader>("pass.vert", "singleColor.frag", ShaderType::PostProcessing));
    ResourceManager::instance().addShader("blend",             std::make_shared<Shader>("pass.vert", "blend.frag", ShaderType::PostProcessing));

    // Other..
    ResourceManager::instance().addShader("mousepicking",      std::make_shared<Shader>("mousepicking.vert", "mousepicking.frag", ShaderType::WeirdStuff));
    ResourceManager::instance().addShader("particle",          std::make_shared<Shader>("particle.vert", "particle.frag", ShaderType::WeirdStuff));
    ResourceManager::instance().addShader("axis",              std::make_shared<Shader>("axisshader.vert", "colorshader.frag", ShaderType::WeirdStuff));
    ResourceManager::instance().addShader("skybox",            std::make_shared<Shader>("skybox", ShaderType::WeirdStuff));
    

    // This function is troublesome...
    // ResourceManager::instance().LoadAssetFiles();

    ResourceManager::instance().addMesh("skybox", "skybox.txt");
    ResourceManager::instance().addMesh("box2", "box2.txt");
    ResourceManager::instance().addMesh("axis", "axis.txt");
    ResourceManager::instance().addMesh("suzanne", "monkey.obj");
    auto ball = ResourceManager::instance().addMesh("ball", "octoball.txt");
    ResourceManager::instance().setupLOD(ball, ResourceManager::instance().addMesh("ball_l2", "octoball_L02.txt"), 2);
    ResourceManager::instance().setupLOD(ball, ResourceManager::instance().addMesh("ball_l1", "octoball_L01.txt"), 1);
    ResourceManager::instance().addMesh("camera", "camera.obj");


    ResourceManager::instance().loadWav("Caravan_mono", std::string{gsl::soundsFilePath}.append("Caravan_mono.wav"));
    ResourceManager::instance().loadWav("explosion", std::string{gsl::soundsFilePath}.append("explosion.wav"));
    ResourceManager::instance().loadWav("laser", std::string{gsl::soundsFilePath}.append("laser.wav"));
    ResourceManager::instance().loadWav("stereo", std::string{gsl::soundsFilePath}.append("stereo.wav"));

    ResourceManager::instance().addTexture("skybox", "skyboxSpaceBoring.bmp", GL_TEXTURE_CUBE_MAP);
    ResourceManager::instance().addTexture("cow", "cow.bmp", GL_TEXTURE_2D);

    entityManager = std::make_shared<EntityManager>();
}

World& World::getWorld()
{
    // Must check for this, because this should never be the case
    assert(mWorldInstance != nullptr);
    return *mWorldInstance;
}

CameraComponent* World::getCurrentCamera(bool currentlyPlaying)
{
    for(auto& camera : getEntityManager()->getCameraComponents())
    {
        if(currentlyPlaying ^ camera.isEditorCamera)
        {
            return &camera;
        }
    }
    return nullptr;
}

void World::initBlankScene()
{
    mCurrentScene = std::make_unique<Scene>(this);

    mCurrentScene->initBlankScene();
    mCurrentScene->initCustomObjects();
    updateSceneName("Blank scene");
}

void World::saveScene(const std::string& path)
{
    if(mCurrentScene)
    {
        mCurrentScene->SaveToFile(path);
        updateSceneName(mCurrentScene->name);
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

    updateSceneName(mCurrentScene->name);
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

        updateSceneName(mCurrentScene->name);
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

World::~World()
{
    mWorldInstance = nullptr;
}

