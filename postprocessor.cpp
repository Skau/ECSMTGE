#include "postprocessor.h"
#include "renderer.h"

Postprocessor::Postprocessor(Renderer *renderer)
{
    mRenderer = renderer;
    init();
}

void Postprocessor::init()
{
    if (mRenderer != nullptr && mRenderer->mContext->isValid())
    {
        initializeOpenGLFunctions();

        glGenFramebuffers(2, mPingpong);
        glGenTextures(2, mRenderTextures);
        for (unsigned int i{0}; i < 2; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mPingpong[i]);
            glBindTexture(GL_TEXTURE_2D, mRenderTextures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mRenderer->width(), mRenderer->height(), 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTextures[i], 0);

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "Postprocessor framebuffer failed to be created!" << std::endl;
                glDeleteFramebuffers(2, mPingpong);
                glDeleteTextures(2, mRenderTextures);
                return;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // Create renderquad
        float quadVertices[] = {
            //    positions   texture Coords
            -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.f, 1.0f,
        };
        // plane VAO setup
        GLuint mQuadVBO;
        glGenVertexArrays(1, &mScreenSpacedQuadVAO);
        glGenBuffers(1, &mQuadVBO);
        glBindVertexArray(mScreenSpacedQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        mInitialized = true;
    }
}

GLuint Postprocessor::input() const
{
    return mPingpong[0];
}

GLuint Postprocessor::output() const
{
    return mPingpong[mLastUsedBuffer];
}

void Postprocessor::Render()
{
    // Reset so that we start at the first ping-pong buffer
    mLastUsedBuffer = 0;

    if (!steps.empty() && !passThroughShader)
    {
        for (auto setting = steps.begin(); setting != steps.end(); ++setting, mLastUsedBuffer = !mLastUsedBuffer)
        {
            // If last render, use default framebuffer; else use the one we didn't use last.
            glBindFramebuffer(GL_FRAMEBUFFER, (outputToDefault && setting + 1 == steps.end()) ? 0 : mPingpong[!mLastUsedBuffer]);

            auto shader = setting->shader;
            if (!shader)
                shader = passThroughShader;

            shader->use();

            // Send relevant uniforms.
            // F.eks. Time.

            renderQuad();
        }
    }
    else
    {
        if (!passThroughShader)
        {
            std::cout << "Postprocessor render step skipped because of missing default shader" << std::endl;
        }

        if (outputToDefault)
        {
            // If no postprocess steps, just blit framebuffer onto default framebuffer.
            glBindFramebuffer(GL_READ_FRAMEBUFFER, mPingpong[0]);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, mRenderer->width(), mRenderer->height(), 0, 0, mRenderer->width(), mRenderer->height(),
                              GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |  GL_STENCIL_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
}

void Postprocessor::renderQuad()
{
    glBindVertexArray(mScreenSpacedQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

Postprocessor::~Postprocessor()
{
    if (mInitialized)
    {
        glDeleteFramebuffers(2, mPingpong);
        glDeleteTextures(2, mRenderTextures);
        glDeleteVertexArrays(1, &mScreenSpacedQuadVAO);
        mInitialized = false;
    }
}
