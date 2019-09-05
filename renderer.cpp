#include "innpch.h"
#include "renderer.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <chrono>
#include <QTime>
#include <QCoreApplication>
#include <QThread>

#include "Renderables/xyz.h"
#include "Renderables/octahedronball.h"
#include "Renderables/skybox.h"
#include "Renderables/billboard.h"
#include "Renderables/trianglesurface.h"
#include "Renderables/objmesh.h"
#include "Renderables/light.h"
#include "Shaders/colorshader.h"
#include "Shaders/textureshader.h"
#include "Shaders/phongshader.h"

#include "resourcemanager.h"


Renderer::Renderer()
{
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setSamples(8);
    format.setSwapInterval(0);

    qDebug() << "Requesting surface format: " << format;

    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    mContext = new QOpenGLContext(this);
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }
}

Renderer::~Renderer()
{
}

void Renderer::init()
{
    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);     //draws only front side of models - usually what you want

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    //Print render version info:
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

    startOpenGLDebugger();

    initDone();
}

///Called each frame - doing the rendering
void Renderer::render(const std::vector<VisualObject*>& objects, double deltaTime)
{
    if(isExposed())
    {
        mCurrentCamera->update(deltaTime);

        mContext->makeCurrent(this);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto visObject: objects)
        {
            visObject->draw();
            //        checkForGLerrors();
        }

        checkForGLerrors();

        mContext->swapBuffers(this);
    }
}

void Renderer::render(std::vector<Render> renders, std::vector<Transform> transforms, double deltaTime)
{
    /* Note: For å gjøre dette enda raskere kunne det vært
     * mulig å gjøre at dataArraysene alltid resizer til nærmeste
     * tall delelig på 8, også kan man unwrappe denne loopen til å
     * gjøre 8 parallelle handlinger. Kan mulig gjøre prosessen raskere.
     */

    if(isExposed())
    {
        mContext->makeCurrent(this);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mCurrentCamera->update(deltaTime);

        auto transIt = transforms.begin();
        auto renderIt = renders.begin();

        bool transformShortest = transforms.size() < renders.size();

        bool _{true};

        // cause normal while (true) loops are so outdated
        for ( ;_; )
        {
            if (transformShortest)
            {
                if (transIt == transforms.end())
                    break;
            }
            else
            {
                if (renderIt == renders.end())
                    break;
            }

            // Increment lowest index
            if (!transIt->valid || transIt->entityId < renderIt->entityId)
            {
                ++transIt;
            }
            else if (!renderIt->valid || renderIt->entityId < transIt->entityId)
            {
                ++renderIt;
            }
            else
            {
                // They are the same, draw.
                if(!renderIt->isVisible)
                {
                    // Increment all
                    ++transIt;
                    ++renderIt;
                    continue;
                }

                // Entity can be drawn. Draw.

                auto meshData = renderIt->meshData;

                glBindVertexArray(meshData.mVAO);

                auto shader = meshData.mMaterial.mShader;
                if(!shader)
                {
                    shader = ResourceManager::instance()->getShader("plain");
                }

                gsl::mat4 matrix;
                matrix.setToIdentity();
                matrix.translate(transIt->position);
                matrix.scale(transIt->scale);

                glUseProgram(shader->getProgram());

                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "mMatrix"), 1, true, matrix.constData());
                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "vMatrix"), 1, true, mCurrentCamera->mViewMatrix.constData());
                glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "pMatrix"), 1, true, mCurrentCamera->mProjectionMatrix.constData());

                if(meshData.mIndicesCount > 0)
                {
                    glDrawElements(meshData.mRenderType, static_cast<GLsizei>(meshData.mIndicesCount), GL_UNSIGNED_INT, nullptr);
                }
                else
                {
                    glDrawArrays(meshData.mRenderType, 0, static_cast<GLsizei>(meshData.mVerticesCount));
                }
                // Increment all
                ++transIt;
                ++renderIt;
            }
        }
    }

    checkForGLerrors();

    mContext->swapBuffers(this);
}

void Renderer::setupCamera()
{
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(gsl::Vector3D(1.f, 1.f, 4.4f));
    ResourceManager::instance()->getShader("plain")->setCurrentCamera(mCurrentCamera);
    ResourceManager::instance()->getShader("texture")->setCurrentCamera(mCurrentCamera);
    ResourceManager::instance()->getShader("phong")->setCurrentCamera(mCurrentCamera);
}


void Renderer::exposeEvent(QExposeEvent *)
{
    if(!isInitialized)
    {
        init();
        isInitialized = true;
    }

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    mAspectratio = static_cast<float>(width()) / height();
    mCurrentCamera->mProjectionMatrix.perspective(45.f, mAspectratio, 1.f, 100.f);
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void Renderer::toggleWireframe()
{
    mWireframe = !mWireframe;
    if (mWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //turn on wireframe mode
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //turn off wireframe mode
        glEnable(GL_CULL_FACE);
    }
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void Renderer::checkForGLerrors()
{
    if(mOpenGLDebugLogger)
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            qDebug() << "glGetError returns " << err;
        }
    }
}

/// Tries to start the extended OpenGL debugger that comes with Qt
void Renderer::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->getContext();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
        }

        if(mOpenGLDebugLogger)
            mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::APISource, QOpenGLDebugMessage::OtherType, QOpenGLDebugMessage::NotificationSeverity);
    }
}

void Renderer::setCameraSpeed(float value)
{
    mCameraSpeed += value;

    //Keep within min and max values
    if(mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}

void Renderer::handleInput(double deltaTime)
{
    //Camera
    mCurrentCamera->setSpeed(0.f);  //cancel last frame movement
    if(mInput.RMB)
    {
        if(mInput.W)
            mCurrentCamera->setSpeed(-mCameraSpeed);
        if(mInput.S)
            mCurrentCamera->setSpeed(mCameraSpeed);
        if(mInput.D)
            mCurrentCamera->moveRight(mCameraSpeed);
        if(mInput.A)
            mCurrentCamera->moveRight(-mCameraSpeed);
        if(mInput.Q)
            mCurrentCamera->updateHeigth(-mCameraSpeed);
        if(mInput.E)
            mCurrentCamera->updateHeigth(mCameraSpeed);
    }
    else
    {
        //        if(mInput.W)
        //            mLight->mMatrix.translateZ(-mCameraSpeed);
        //        if(mInput.S)
        //            mLight->mMatrix.translateZ(mCameraSpeed);
        //        if(mInput.D)
        //            mLight->mMatrix.translateX(mCameraSpeed);
        //        if(mInput.A)
        //            mLight->mMatrix.translateX(-mCameraSpeed);
        //        if(mInput.Q)
        //            mLight->mMatrix.translateY(mCameraSpeed);
        //        if(mInput.E)
        //            mLight->mMatrix.translateY(-mCameraSpeed);
    }
}

void Renderer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        escapeKeyPressed();
    }

    //    You get the keyboard input like this
    if(event->key() == Qt::Key_W)
    {
        mInput.W = true;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = true;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = true;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = true;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = true;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = true;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = true;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = true;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = true;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = true;
    }
    if(event->key() == Qt::Key_U)
    {
    }
    if(event->key() == Qt::Key_O)
    {
    }
}

void Renderer::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        mInput.W = false;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = false;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = false;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = false;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = false;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = false;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = false;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = false;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = false;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = false;
    }
    if(event->key() == Qt::Key_U)
    {
    }
    if(event->key() == Qt::Key_O)
    {
    }
}

void Renderer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void Renderer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void Renderer::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput.RMB)
    {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void Renderer::mouseMoveEvent(QMouseEvent *event)
{
    if (mInput.RMB)
    {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
        mMouseXlast = event->pos().x() - mMouseXlast;
        mMouseYlast = event->pos().y() - mMouseYlast;

        if (mMouseXlast != 0)
            mCurrentCamera->yaw(mCameraRotateSpeed * mMouseXlast);
        if (mMouseYlast != 0)
            mCurrentCamera->pitch(mCameraRotateSpeed * mMouseYlast);
    }
    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}
