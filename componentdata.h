#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "innpch.h"
#include "meshdata.h"

// Something like this?

struct Component
{
    unsigned int entityId;
    bool valid : 1;

    Component() : valid(false) {}
};

struct EntityData : public Component
{
    std::string name{};
};

struct Transform : public Component
{
    gsl::Vector3D position{};
    gsl::Vector3D scale{1,1,1};
    gsl::Vector3D rotation{};
};

struct Render : public Component
{
    MeshData meshData{};
    bool isVisible : 1;

    Render() : isVisible(false) {}
};

// .. etc

#endif // COMPONENTS_H
