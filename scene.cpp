#include "scene.h"

#include "Renderables/xyz.h"
#include "Renderables/octahedronball.h"
#include "Renderables/skybox.h"
#include "Renderables/billboard.h"
#include "Renderables/trianglesurface.h"
#include "Renderables/objmesh.h"
#include "Renderables/light.h"

#include "resourcemanager.h"

#include "world.h"
#include "entitymanager.h"

// Just hardcoded scene for now
Scene::Scene(World *world) : mWorld(world)
{
    VisualObject * temp{nullptr};

    temp = new XYZ();
    temp->init();
    temp->setShader(ResourceManager::instance()->getShader("plain"));
    mVisualObjects.push_back(temp);

    // *** Commented it out because it's not showing up? *** //
//    temp = new OctahedronBall(2);
//    temp->init();
//    temp->setShader(ResourceManager::instance()->getShader("plain"));
//    temp->mMatrix.scale(0.5f, 0.5f, 0.5f);
//    temp->mName = "Ball";
//    mVisualObjects.push_back(temp);
//    mPlayer = temp;

    temp = new SkyBox();
    temp->init();
    temp->setShader(ResourceManager::instance()->getShader("texture"));
    temp->mMaterial.setTextureUnit(2);
    temp->mMatrix.scale(15.f);
    temp->mName = "Cube";
    mVisualObjects.push_back(temp);

    // *** Not using because it needs the renderer (not setup yet) *** //
//    temp = new BillBoard();
//    temp->init();
//    temp->setShader(ResourceManager::instance()->getShader("texture"));
//    temp->mMatrix.translate(4.f, 0.f, -3.5f);
//    temp->mName = "Billboard";
//    temp->mRenderWindow = this;
//    temp->mMaterial.setTextureUnit(1);
//    temp->mMaterial.mObjectColor = gsl::Vector3D(0.7f, 0.6f, 0.1f);
//    dynamic_cast<BillBoard*>(temp)->setConstantYUp(true);
//    mVisualObjects.push_back(temp);


    // *** Not using this because it has to be passed to the renderer (not setup yet) *** //
//    mLight = new Light();
//    temp = mLight;
//    temp->init();
//    temp->setShader(mShaderProgram[1]);
//    temp->mMatrix.translate(2.5f, 3.f, 0.f);
//    //    temp->mMatrix.rotateY(180.f);
//    temp->mName = "light";
//    temp->mRenderWindow = this;
//    temp->mMaterial.setTextureUnit(0);
//    temp->mMaterial.mObjectColor = gsl::Vector3D(0.1f, 0.1f, 0.8f);
//    mVisualObjects.push_back(temp);

    // *** The mesh is weird on runtime now for some reason *** //
    temp = new TriangleSurface("box2.txt");
    temp->init();
    temp->mMatrix.rotateY(180.f);
    temp->setShader(ResourceManager::instance()->getShader("plain"));
    mVisualObjects.push_back(temp);

    // *** No light so this is commented out *** //
//    static_cast<PhongShader*>(mShaderProgram[2])->setLight(mLight);

    // *** Because of no light this is just black *** //
    temp = new ObjMesh("monkey.obj");
    temp->setShader(ResourceManager::instance()->getShader("phong"));
    temp->init();
    temp->mName = "Monkey";
    temp->mMatrix.scale(0.5f);
    temp->mMatrix.translate(3.f, 2.f, -2.f);
    mVisualObjects.push_back(temp);

    auto entityManager = world->getEntityManager();

    auto entity = entityManager->createEntity();
    entityManager->addComponent<Transform>(entity);
    entityManager->addComponent<Render>(entity);

    auto entity1 = entityManager->createEntity();
    entityManager->addComponent<Transform>(entity1);
    entityManager->addComponent<Render>(entity1);

    auto entity2 = entityManager->createEntity();
    entityManager->addComponent<Transform>(entity2);
    entityManager->addComponent<Render>(entity2);

    auto entity3 = entityManager->createEntity();
    entityManager->addComponent<Transform>(entity3);
    entityManager->addComponent<Render>(entity3);

    auto [TransformComponent, RenderComponent] = entityManager->createEntiy<Transform, Render>();

}
