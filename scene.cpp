#include "scene.h"
#include "world.h"

#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonObject>

Scene::Scene(World* world)
    : mWorld(world)
{   
}

Scene::~Scene()
{
    mWorld = nullptr;
}

void Scene::initBlankScene()
{
    auto entityManager = mWorld->getEntityManager();

    // Editor cam
    auto camera = entityManager->createEntity("EditorCam");
    auto [editorTrans, editorCam, editorInput] = entityManager->addComponent<TransformComponent, CameraComponent, InputComponent>(camera);
    editorTrans.setPosition(gsl::vec3{-1.f, 3.f, 10.f});
    editorTrans.setRotation({1.f, .27f, .1f, 0.f});
    editorInput.isCurrentlyControlled = true;
    editorCam.isEditorCamera = true;
    editorCam.yaw = 10.f;
    editorCam.pitch = 30.f;
    for(auto& info : entityManager->getEntityInfos())
    {
        if(info.entityId == camera)
        {
            info.shouldShowInEditor = false;
        }
    }

    // Game cam
    camera = entityManager->createEntity("GameCam");
    auto [gameTrans, gameCam, gameMesh, gameInput] = entityManager->addComponent<TransformComponent, CameraComponent, MeshComponent, InputComponent>(camera);
    gameTrans.setPosition(gsl::vec3{0.f, 0.f, 5.f});
    if(auto meshData = ResourceManager::instance()->getMesh("camera"))
    {
        gameMesh.meshData = *meshData;
        gameMesh.isVisible = true;
    }

    // Light
    auto entity = entityManager->createEntity("light");
    entityManager->addComponent<TransformComponent, DirectionalLightComponent>(entity);
}

bool Scene::LoadFromFile(const std::string& path)
{
    QFile file(QString::fromStdString(path));
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR Scene.load(): Failed to open file at specified path!";
        return false;
    }


   QFileInfo info(QString::fromStdString(path));
   auto baseName = info.baseName();
   if(baseName.contains("temp"))
        baseName = baseName.replace("temp", "");
   name = baseName.toStdString();

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if(!document.isObject())
    {
        qDebug() << "Load Error: Wrong formatting";
        return false;
    }

    QJsonObject mainObject = document.object();
    if(mainObject.isEmpty())
    {
        qDebug() << "Load Error: File is empty.";
        return false;
    }

    filePath = path;

    mWorld->clearEntities();
    auto entityManager = mWorld->getEntityManager();

    // Iterate all entities
    for(auto entityRef : mainObject["Entities"].toArray())
    {
        // Current entity
        auto entityObj = entityRef.toObject();

        // Create the entity
        auto entity = entityManager->createEntity(entityObj["Name"].toString().toStdString());
        auto shouldShowInEditor = entityObj["ShowInEditor"].toBool();
        if(!shouldShowInEditor)
        {
            for(auto& info : entityManager->getEntityInfos())
            {
                if(info.entityId == entity)
                {
                    info.shouldShowInEditor = false;
                }
            }
        }

        // Iterate all components
        for(auto compRef : entityObj["Components"].toArray())
        {
            // Current component
            auto compObject = compRef.toObject();

            // Add the component
            auto comp = entityManager->addComponent(entity, static_cast<ComponentType>(compObject["ComponentType"].toInt()));

            // Inititalize the component
            comp->fromJSON(compObject);
        }
    }

    return true;
}

void Scene::SaveToFile(const std::string& path)
{
    QFile file(QString::fromStdString(path));
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Save error: Failed to open file at specified path!";
        return;
    }

    filePath = path;

    const auto& entityManager = mWorld->getEntityManager();
    const auto& entityInfos = entityManager->getEntityInfos();


    QJsonObject mainObject;

    QJsonArray entityArray;

    // Iterate all entities
    for(auto& entityInfo : entityInfos)
    {
        // Get entit
        std::vector<Component*> components;
        if(!entityManager->getAllComponents(entityInfo.entityId, components))
        {
            // No components found
            continue;
        }


        QJsonArray compArray;
        for(auto comp : components)
        {
            // Skip entity info
            if(comp->type == ComponentType::Other)
                continue;
            compArray.push_back(comp->toJSON());
        }

        QJsonObject entityObject;
        entityObject.insert("Components", compArray);
        entityObject.insert("Name", QJsonValue(entityInfo.name.c_str()));
        entityObject.insert("ShowInEditor", QJsonValue(entityInfo.shouldShowInEditor));
        entityArray.push_back(entityObject);
    }

    mainObject.insert("Entities", entityArray);

    QJsonDocument document(mainObject);
    file.write(document.toJson());
    file.close();
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
        transform.setPosition(gsl::vec3(i*2.f, 0, 0));
    }
}
