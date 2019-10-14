#include "qentity.h"
#include "qcomponentdata.h"

QEntity::QEntity(unsigned int _ID, EntityManager* _entityManager, QObject* parent)
    :  QObject(parent), entityManager(_entityManager), mID(_ID)
{

}

void QEntity::setPosition(float x, float y, float z)
{
    auto comp = entityManager->getComponent<TransformComponent>(mID);
    if(comp)
    {
        comp->setPosition(gsl::vec3(x, y, z));
    }
}


void QEntity::setScale(float x, float y, float z)
{
    auto comp = entityManager->getComponent<TransformComponent>(mID);
    if(comp)
    {
        comp->setScale(gsl::vec3(x, y, z));
    }
}

void QEntity::setScale(float value)
{
    auto comp = entityManager->getComponent<TransformComponent>(mID);
    if(comp)
    {
        comp->setScale(gsl::vec3(value));
    }
}

void QEntity::setVelocity(float x, float y, float z)
{
    auto comp = entityManager->getComponent<PhysicsComponent>(mID);
    if(comp)
    {
        comp->velocity = gsl::vec3(x, y, z);
    }
}

QObject* QEntity::getComponent(const QString& name)
{
    if(name == "mesh")
    {
        return new QMeshComponent(*entityManager->getComponent<MeshComponent>(mID));
    }
    return nullptr;
}

