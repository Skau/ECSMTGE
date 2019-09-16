#include "componentdata.h"
#include <stack>


void TransformComponent::addPosition(const gsl::vec3 &pos)
{
    position += pos;
    updated = true;
}

void TransformComponent::addRotation(const gsl::quat &rot)
{

}

void TransformComponent::addScale(const gsl::vec3 &scale)
{

}

void TransformComponent::setPosition(const gsl::vec3 &pos)
{

}

void TransformComponent::setRotation(const gsl::quat &rot)
{

}

void TransformComponent::setScale(const gsl::vec3 &scale)
{

}
