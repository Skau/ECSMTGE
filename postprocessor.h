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

/** Postprocessing helper class.
 * An instantiated class that constructs a pair of ping pong framebuffers
 * and has a fairly straightforward way of iterating between the framebuffers.
 *
 * Steps is an array of shaders and other settings that the postprocessor will use
 * each draw call between the ping pong buffers.
 * Render(void) iterates through steps, drawing a screen sized quad for each one and
 * switches back and forth between the ping pong shaders.
 *
 * @brief Postprocessing helper class.
 */
class Postprocessor : protected QOpenGLFunctions_4_1_Core
{
public:
    /** Postprocessor step instruction.
     * Each step includes a material (with a shader) that will be used
     * to render the whole screen onto a screen sized
     * quad. Vertex shader can be pass.vert or a custom
     * shader and fragment shader can be any custom shader
     * but requires:
     * uniform sampler2D fbt;
     * - to work. fbt is the last framebuffer texture, sampling from
     * this will get you the last step.
     *
     * @brief Postprocessor step instruction.
     */
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

    bool autoUpdateSize = true;


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

    /** Input framebuffer of postprocessor.
     * Use this when sending/blit-ing to the postprocessor.
     * @brief Input framebuffer of postprocessor.
     * @return Index of input postprocessor framebuffer.
     */
    GLuint input() const;
    /** Output framebuffer of postprocessor.
     * The last framebuffer that was rendered to.
     * Use this when receiving/blit-ing from the postprocessor.
     * @brief Output framebuffer of postprocessor.
     * @return Index of output framebuffer.
     */
    GLuint output() const;
    /** Input texture of postprocessor framebuffer.
     * Textures are bound to each framebuffer and this
     * is the texture bound to the input() framebuffer.
     * @brief Input texture of postprocessor framebuffer.
     * @return Index of texture bound to input framebuffer.
     */
    GLuint inputTex() const;
    /** Output texture of postprocessor framebuffer.
     * Textures are bound to each framebuffer and this
     * is the texture bound to the output() framebuffer.
     * @brief Output texture of postprocessor framebuffer.
     * @return Index of texture bound to output framebuffer.
     */
    GLuint outputTex() const;

    int width() const;
    int height() const;
    void setSize(gsl::ivec2 size);
    // For retina screens which uses higher resolutions textures
    // and then downsamples on GPU.
    double pixelSize() const;

    enum BLENDMODE : int {
        ADDITIVE = 0,
        MULTIPLY = 1
    };

    // Blending
    Postprocessor& add(Postprocessor& other, BLENDMODE blendmode = BLENDMODE::ADDITIVE);
    Postprocessor& operator+=(Postprocessor& other);


    /** Renders each step in the "steps" array and iterates between the ping pong buffers.
     * If outputToDefault to is set to true, the last step will be rendered directly to the
     * screen (framebuffer 0), otherwise the last step will be rendered to the framebuffer
     * in output().
     * @brief Execute steps.
     */
    void Render();

    // TODO: Should change this to use iterators instead of indexes.
    /** Functions the same as Render(), but allows for custom handling of the loop.
     * Does the same operations as Render() but only performs a single step in "steps" array
     * based on the index sent in.
     * @brief Same as Render() but with custom loop handling.
     * @param index - the current index in the step array to perform.
     * @return Returns the next step index if step was performed or the
     * same index otherwise.
     */
    unsigned int RenderStep(unsigned int index);

    // Resets ping pong buffers and binds to postprocessor framebuffer
    void clear();

    ~Postprocessor();
};

#endif // POSTPROCESSOR_H
