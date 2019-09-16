/** cpp file for extra stuff entitymanager doesn't
 * need to have in it's header.
 */

#include "entitymanager.h"

void EntityManager::addTransformPos(unsigned int eID, const gsl::vec3 &pos)
{
    // Iterative version. Easier to read, but I think treeiterator is slower.
//    for (auto it{treeBegin(eID)}; it != treeEnd(); ++it)
//    {
//        it->addPosition(pos);
//    }

    // Recursive version. Should be faster, actually.
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        comp->addPosition(pos);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            addTransformPos(*it, pos);
    }
}

void EntityManager::addTransformRot(unsigned int eID, const gsl::quat &rot)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        comp->addRotation(rot);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            addTransformRot(*it, rot);
    }
}

void EntityManager::addTransformScale(unsigned int eID, const gsl::vec3 &scale)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        comp->addScale(scale);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            addTransformScale(*it, scale);
    }
}

void EntityManager::setTransformPos(unsigned int eID, const gsl::vec3 &pos)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        comp->setPosition(pos);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            setTransformPos(*it, pos);
    }
}

void EntityManager::setTransformRot(unsigned int eID, const gsl::quat &rot)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        comp->setRotation(rot);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            setTransformRot(*it, rot);
    }
}

void EntityManager::setTransformScale(unsigned int eID, const gsl::vec3 &scale)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        comp->setScale(scale);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            setTransformScale(*it, scale);
    }
}

gsl::vec3 EntityManager::getTransformPos(unsigned int eID)
{
    auto comp = getComponent<TransformComponent>(eID);
    return (comp != nullptr) ? comp->position : gsl::vec3{};
}

gsl::quat EntityManager::getTransformRot(unsigned int eID)
{
    auto comp = getComponent<TransformComponent>(eID);
    return (comp != nullptr) ? comp->rotation : gsl::quat{};
}

gsl::vec3 EntityManager::getTransformScale(unsigned int eID)
{
    auto comp = getComponent<TransformComponent>(eID);
    return (comp != nullptr) ? comp->scale : gsl::vec3{};
}
