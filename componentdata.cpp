#include "componentdata.h"
#include <stack>


void TransformComponent::addPosition(const gsl::vec3 &pos)
{
    position += pos;
    updated = true;
}

void TransformComponent::addRotation(const gsl::quat &rot)
{
    rotation *= rot;
    updated = true;
}

void TransformComponent::addScale(const gsl::vec3 &scl)
{
    scale += scl;
    updated = true;
}

void TransformComponent::setPosition(const gsl::vec3 &pos)
{
    position = pos;
    updated = true;
}

void TransformComponent::setRotation(const gsl::quat &rot)
{
    rotation = rot;
    updated = true;
}

void TransformComponent::setScale(const gsl::vec3 &scl)
{
    scale = scl;
    updated = true;
}
