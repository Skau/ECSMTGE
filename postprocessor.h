#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <QOpenGLFunctions_4_1_Core>
#include <memory>
#include <vector>
#include "innpch.h"
#include "componentdata.h"
#include <variant>

// Forward declarations
class Renderer;

class Postprocessor : protected QOpenGLFunctions_4_1_Core
{
public:
    struct Setting {
        std::shared_ptr<Material> material;

        // Other probs useful data

        // bool useStencil = false;
        unsigned char stencilValue{0};

        Setting(const std::shared_ptr<Material>& _mat = nullptr, unsigned char _stencilValue = 0)
            : material{_mat}, stencilValue{_stencilValue}
        {}
        Setting(const Setting& setting)
            : material{setting.material}, stencilValue{setting.stencilValue}
        {}
        Setting(Setting&& setting)
            : material{std::move(setting.material)}, stencilValue{setting.stencilValue}
        {}

    };


    // Postprocessor settings
    std::shared_ptr<Material> passThroughMaterial;
    std::vector<Setting> steps;
    /** Whether or not to output to default framebuffer after renderprocess
     * or to output to "output" framebuffer.
     * Can be turned off if there's a need to render more after postprocessing.
     */
    bool outputToDefault = true;

    /** When depth sampling is disabled a renderbuffer is used
     * for depth storage, which is faster but cannot be sampled
     * from. Otherwise uses a texture.
     */
    bool depthSampling = false;


private:
    bool mInitialized{false};
    Renderer *mRenderer{nullptr};

    /** What textureformat to use internally for framebuffers.
     * Defaults to RGBA which will clamp colors between 0 and 1.
     */
    int mTextureFormat = GL_RGBA;

    // Ping pong framebuffers.
    GLuint mPingpong[2];
    GLuint mRenderTextures[2];
    GLuint mDepthStencilBuffer[2];
    bool depthStencilUsingRenderbuffer = true;
    int mScrWidth{0}, mScrHeight{0};
    double mRetinaScale{1.0};
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

    void setTextureFormat(int format);

    GLuint input() const;
    GLuint output() const;

    enum BLENDMODE : int {
        ADDITIVE = 0,
        MULTIPLY = 1
    };

    // Blending
    Postprocessor& add(Postprocessor& other, BLENDMODE blendmode = BLENDMODE::ADDITIVE);
    Postprocessor& operator+=(Postprocessor& other);


    // Renders every post process effect over the screen
    void Render();

    // For custom handling of renderloop
    // Send in the loopindex and returns the new loopindex if it looped or the same if it didn't.
    unsigned int RenderStep(unsigned int index);

    // Resets ping pong buffers and binds to postprocessor framebuffer
    void clear();

    ~Postprocessor();
};

#endif // POSTPROCESSOR_H
