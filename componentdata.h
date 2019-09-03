#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "math.h"

// Something like this?

struct Component
{
    unsigned int entityId;
    bool valid{false};
};

struct Transform : public Component
{
    gsl::Vector3D position;
    gsl::Vector3D scale;
    gsl::Vector3D rotation;
};

struct Render : public Component
{

};

// .. etc

#endif // COMPONENTS_H
