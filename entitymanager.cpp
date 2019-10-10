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
        auto delta = pos - getTransformPos(eID);
        comp->addPosition(delta);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            addTransformPos(*it, delta);
    }
}

void EntityManager::setTransformRot(unsigned int eID, const gsl::quat &rot)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        // Temporary: Needs to find diff instead and add that.
        auto delta = gsl::quat::diff(getTransformRot(eID), rot);
        comp->addRotation(delta);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            addTransformRot(*it, delta);
    }
}

void EntityManager::setTransformScale(unsigned int eID, const gsl::vec3 &scale)
{
    auto comp = getComponent<TransformComponent>(eID);
    if (comp != nullptr)
    {
        auto delta = scale - getTransformScale(eID);
        comp->addScale(delta);
        for (auto it{comp->children.begin()}; it != comp->children.end(); ++it)
            addTransformScale(*it, delta);
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

void EntityManager::UpdateBounds()
{
    if (mTransformComponents.empty() || mMeshComponents.empty())
        return;

    auto meshIt = mMeshComponents.begin();
    for (auto transIt = mTransformComponents.begin(); transIt != mTransformComponents.end(); ++transIt)
    {
        if (transIt->boundsOutdated)
        {
            while (transIt->entityId > meshIt->entityId)
                ++meshIt;

            // Transform doesn't have mesh
            if (transIt->entityId != meshIt->entityId)
            {
                transIt->boundsOutdated = false;
                continue;
            }
            else
            {
                // Find which axis it scales the most in and use it as a uniform scale.
                float biggestScale = 0.f;
                for (auto i{0}; i < 3; ++i)
                    biggestScale = (biggestScale < *(&transIt->scale.x + i)) ? *(&transIt->scale.x + i) : biggestScale;

                meshIt->bounds = {meshIt->meshData.bounds.centre, meshIt->meshData.bounds.radius * biggestScale};
                transIt->boundsOutdated = false;
            }
        }
    }
}
