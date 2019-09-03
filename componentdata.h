#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "gsl_math.h"

// Something like this?

struct Transform
{
    gsl::Vector3D position;
    gsl::Vector3D scale;
    gsl::Vector3D rotation;
};

struct Render
{

};

// .. etc

#endif // COMPONENTS_H
