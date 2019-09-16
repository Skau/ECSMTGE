/** cpp file for extra stuff entitymanager doesn't
 * need to have in it's header.
 */

#include "entitymanager.h"

void EntityManager::addTransformPos(unsigned int eID, const gsl::vec3 &pos)
{
    for (auto it{treeBegin(eID)}; it != treeEnd(); ++it)
    {
        it->addPosition(pos);
    }
}

void EntityManager::addTransformRot(unsigned int eID, const gsl::quat &rot)
{

}

void EntityManager::addTransformScale(unsigned int eID, const gsl::vec3 &scale)
{

}

void EntityManager::setTransformPos(unsigned int eID, const gsl::vec3 &pos)
{

}

void EntityManager::setTransformRot(unsigned int eID, const gsl::quat &rot)
{

}

void EntityManager::setTransformScale(unsigned int eID, const gsl::vec3 &scale)
{

}
