#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "math.h"
#include "resourcemanager.h"
#include <memory>

// Something like this?

struct Component
{
    unsigned int entityId;
    bool valid{false};
};

struct Transform : public Component
{
    gsl::Vector3D position{0.f, 0.f, 0.f};
    gsl::Vector3D scale{1.f, 1.f, 1.f};
    gsl::Vector3D rotation{0.f, 0.f, 0.f};

    /* Possible to calculate from the other values,
     * but more efficient to just calculate it
     * whenever it is changed
     */
    gsl::mat4 modelMatrix;
};

struct Render : public Component
{
    GLuint VAO;
};

struct Material : public Component
{
    Material()
    {
        if (mShader == nullptr)
            mShader = ResourceManager::instance()->getShader("plain");
    }
    Material(gsl::vec3 color, GLuint texUnit, Shader* shader)
        : mObjectColor{color}, mTextureUnit{texUnit}, mShader{shader}
    {
        if (mShader == nullptr)
            mShader = ResourceManager::instance()->getShader("plain");
    }

    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0};     //the actual texture to put into the uniform
    Shader *mShader{nullptr};
};

// .. etc

#endif // COMPONENTS_H
