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

    particleShader = ResourceManager::instance()->getShader("particle");
    assert(particleShader);
}

void ParticleSystem::emitParticles(const CameraComponent &camera, ParticleComponent &particles, float time)
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
