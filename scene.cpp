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

    // Camera:
    auto camera = entityManager->createEntity("mainCam");
    auto [camTrans, camCam, dirLight, camInput] = entityManager->addComponent<TransformComponent, CameraComponent, DirectionalLightComponent, InputComponent>(camera);
    camTrans.setPosition(gsl::vec3{0.f, 0.f, 5.f});
    camInput.isCurrentlyControlled = true;
    camCam.isCurrentActive = true;
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
   name = info.baseName().toStdString();

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
