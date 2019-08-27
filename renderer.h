#ifndef RENDERER_H
#define RENDERER_H

#include <QWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <chrono>
#include "texture.h"
#include "camera.h"
#include "Renderables/visualobject.h"
#include "input.h"

class QOpenGLContext;
class Shader;
class MainWindow;
class Light;
class QTime;

/// This inherits from QWindow to get access to the Qt functionality and
/// OpenGL surface.
/// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
/// This is the same as using glad and glw from general OpenGL tutorials
class Renderer : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    Renderer();
    ~Renderer() override;

    QOpenGLContext* getContext() { return mContext; }

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe();

    void checkForGLerrors();

    bool mInitialized{false};

    void init();
    void handleInput(double deltaTime);
    void render(double deltaTime);

signals:
    void escapeKeyPressed();


private:
    void setCameraSpeed(float value);

    QOpenGLContext *mContext{nullptr};

    Texture *mTexture[4]{nullptr}; //We can hold 4 textures
    Shader *mShaderProgram[4]{nullptr}; //We can hold 4 shaders

    void setupPlainShader(int shaderIndex);
    GLint mMatrixUniform0{-1};
    GLint vMatrixUniform0{-1};
    GLint pMatrixUniform0{-1};

    void setupTextureShader(int shaderIndex);
    GLint mMatrixUniform1{-1};
    GLint vMatrixUniform1{-1};
    GLint pMatrixUniform1{-1};
    GLint mTextureUniform{-1};

    std::vector<VisualObject*> mVisualObjects;

    VisualObject *mPlayer;  //the controllable object
    Light *mLight;

    Camera *mCurrentCamera{nullptr};

    bool mWireframe{false};

    Input mInput;
    float mCameraSpeed{0.01f};
    float mCameraRotateSpeed{0.1f};
    int mMouseXlast{0};
    int mMouseYlast{0};

    float mAspectratio{1.f};

   // MainWindow *mMainWindow{nullptr};    //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void startOpenGLDebugger();

protected:
    //The QWindow that we inherit from has these functions to capture
    // mouse and keyboard. Uncomment to use
    //
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERER_H
