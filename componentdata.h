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

struct Transform : public Component
{
    bool updated{false};
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

struct Camera : public Component
{
    GLuint framebufferTarget;
    gsl::Matrix4x4 viewMatrix;
    gsl::Matrix4x4 projectionMatrix;
};

// .. etc

#endif // COMPONENTS_H
