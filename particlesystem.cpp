#include "particlesystem.h"
#include "resourcemanager.h"
#include <cassert>

ParticleSystem::ParticleSystem()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadvertices), quadvertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid*)(sizeof(float) * 6));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void ParticleSystem::updateParticles(const CameraComponent &camera, const std::vector<TransformComponent> &transforms, const std::vector<ParticleComponent> &particles, float time)
{
    auto transIt = transforms.begin();
    auto partIt = particles.begin();

    bool particlesShortest = particles.size() < transforms.size();

    if (!particleShader)
    {
        particleShader = ResourceManager::instance()->getShader("particle");
        assert(particleShader);
    }

    // cause normal while (true) loops are so outdated
    while (static_cast<bool>("Everything is awesome!"))
    {
        if (particlesShortest)
        {
            if (partIt == particles.end())
                break;
        }
        else
        {
            if (transIt == transforms.end())
                break;
        }

        // Increment lowest index
        if (!transIt->valid || transIt->entityId < partIt->entityId)
        {
            ++transIt;
        }
        else if (!partIt->valid || partIt->entityId < transIt->entityId)
        {
            ++partIt;
        }
        else
        {
            updateParticle(camera, *transIt, *partIt, time);

            // Increment all
            ++transIt;
            ++partIt;
        }
    }
}

void ParticleSystem::updateParticle(const CameraComponent &camera, const TransformComponent &transform, const ParticleComponent &particles, float time)
{
    particleShader->use();

    glUniformMatrix4fv(glGetUniformLocation(particleShader->getProgram(), "vMatrix"), 1, GL_TRUE, camera.viewMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(particleShader->getProgram(), "pMatrix"), 1, GL_TRUE, camera.projectionMatrix.constData());
    if (auto uniform = glGetUniformLocation(particleShader->getProgram(), "sTime"))
        glUniform1f(uniform, time);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
}
