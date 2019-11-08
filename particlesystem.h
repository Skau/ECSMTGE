#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "componentdata.h"
#include <QOpenGLFunctions_4_1_Core>

class ParticleSystem : QOpenGLFunctions_4_1_Core
{
public:
    ParticleSystem();

    void updateParticles(const CameraComponent& camera, const std::vector<TransformComponent>& transforms,
                         const std::vector<ParticleComponent>& particles, float time = 0.f);
    void updateParticle(const CameraComponent &camera, const TransformComponent& transform, const ParticleComponent& particles, float time = 0.f);

    ~ParticleSystem();
private:
    static constexpr float quadvertices[]{
        // Position         // Color            // UV
        -1.f, -1.f, 0.f,    1.f, 0.f, 0.f,      0.f, 0.f,
        1.f, -1.f, 0.f,     0.f, 1.f, 0.f,      1.f, 0.f,
        1.f, 1.f, 0.f,      0.f, 0.f, 1.f,      1.f, 1.f,

        -1.f, -1.f, 0.f,    1.f, 0.f, 0.f,      0.f, 0.f,
        1.f, 1.f, 0.f,      0.f, 0.f, 1.f,      1.f, 1.f,
        -1.f, 1.f, 0.f,     1.f, 1.f, 0.f,      0.f, 1.f
    };

    GLuint quadVAO, quadVBO;
    std::shared_ptr<Shader> particleShader;
};

#endif // PARTICLESYSTEM_H
