#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <QOpenGLFunctions_4_1_Core>
#include <memory>
#include <vector>
#include "innpch.h"
#include "componentdata.h"

// Forward declarations
class Renderer;

class Postprocessor : protected QOpenGLFunctions_4_1_Core
{
public:
    struct Setting {
        std::shared_ptr<Shader> shader;
        // Other probs useful data
    };


    // Postprocessor settings
    std::shared_ptr<Shader> passThroughShader;
    std::vector<Setting> steps;
    /* Whether or not to output to default framebuffer after renderprocess
     * or to output to "output" framebuffer.
     * Can be turned off if there's a need to render more after postprocessing.
     */
    bool outputToDefault = true;

    /* When depth sampling is disabled a renderbuffer is used
     * for depth storage, which is faster but cannot be sampled
     * from. Otherwise uses a texture.
     */
    bool depthSampling = false;


private:
    bool mInitialized{false};
    Renderer *mRenderer{nullptr};

    // Ping pong framebuffers.
    GLuint mPingpong[2];
    GLuint mRenderTextures[2];
    GLuint depthStencilBuffer[2];
    bool depthStencilUsingRenderbuffer = true;
    int mScrWidth{0}, mScrHeight{0};
    unsigned char mLastUsedBuffer{0};

    GLuint mScreenSpacedQuadVAO;
    void renderQuad();
    // Returns true if screen sizes has changed.
    bool outdatedRatio() const;
    void updateRatio();

    // Manually destroys and recreates the framebuffers and textures.
    void recreateBuffers();

public:
    Postprocessor(Renderer* renderer);
    void init();

    GLuint input() const;
    GLuint output() const;

    // Renders every post process effect over the screen
    void Render();

    void clear();

    ~Postprocessor();
};

#endif // POSTPROCESSOR_H
