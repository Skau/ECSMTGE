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

#include "mainwindow.h"

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

Renderer::Renderer() : mInitialized(false)
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
    create();
}

Renderer::~Renderer()
{
    for (int i = 0; i < 4; ++i) {
        if (mShaderProgram[i])
            delete mShaderProgram[i];
    }
}

/// Sets up the general OpenGL stuff and the buffers needed to render a triangle
void Renderer::init()
{
    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Print render version info:
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);     //draws only front side of models - usually what you want -
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);    //color used in glClear GL_COLOR_BUFFER_BIT

    //Compile shaders:
    mShaderProgram[0] = new ColorShader("plainshader");
    qDebug() << "Plain shader program id: " << mShaderProgram[0]->getProgram();
    mShaderProgram[1]= new TextureShader("textureshader");

    qDebug() << "Texture shader program id: " << mShaderProgram[1]->getProgram();
    mShaderProgram[2]= new PhongShader("phongshader");
    qDebug() << "Phong shader program id: " << mShaderProgram[2]->getProgram();

    //**********************  Texture stuff: **********************

    mTexture[0] = new Texture("white.bmp");
    mTexture[1] = new Texture("hund.bmp", 1);
    mTexture[2] = new Texture("skybox.bmp", 2);

    //Set the textures loaded to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture[0]->id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture[1]->id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mTexture[2]->id());

    //********************** Making the objects to be drawn **********************
    VisualObject * temp{nullptr};

    temp = new XYZ();
    temp->init();
    temp->setShader(mShaderProgram[0]);
    mVisualObjects.push_back(temp);

//    temp = new OctahedronBall(2);
//    temp->init();
//    temp->setShader(mShaderProgram[0]);
//    temp->mMatrix.scale(0.5f, 0.5f, 0.5f);
//    temp->mName = "Ball";
//    mVisualObjects.push_back(temp);
//    mPlayer = temp;

    temp = new SkyBox();
    temp->init();
    temp->setShader(mShaderProgram[1]);
    temp->mMaterial.setTextureUnit(2);
    temp->mMatrix.scale(15.f);
    temp->mName = "Cube";
    mVisualObjects.push_back(temp);

    temp = new BillBoard();
    temp->init();
    temp->setShader(mShaderProgram[1]);
    temp->mMatrix.translate(4.f, 0.f, -3.5f);
    temp->mName = "Billboard";
    temp->mRenderWindow = this;
    temp->mMaterial.setTextureUnit(1);
    temp->mMaterial.mObjectColor = gsl::Vector3D(0.7f, 0.6f, 0.1f);
    dynamic_cast<BillBoard*>(temp)->setConstantYUp(true);
    mVisualObjects.push_back(temp);

    mLight = new Light();
    temp = mLight;
    temp->init();
    temp->setShader(mShaderProgram[1]);
    temp->mMatrix.translate(2.5f, 3.f, 0.f);
    //    temp->mMatrix.rotateY(180.f);
    temp->mName = "light";
    temp->mRenderWindow = this;
    temp->mMaterial.setTextureUnit(0);
    temp->mMaterial.mObjectColor = gsl::Vector3D(0.1f, 0.1f, 0.8f);
    mVisualObjects.push_back(temp);

    //testing triangle surface class
    temp = new TriangleSurface("box2.txt");
    temp->init();
    temp->mMatrix.rotateY(180.f);
    temp->setShader(mShaderProgram[0]);
    mVisualObjects.push_back(temp);

    static_cast<PhongShader*>(mShaderProgram[2])->setLight(mLight);

    //one monkey
    temp = new ObjMesh("monkey.obj");
    temp->setShader(mShaderProgram[2]);
    temp->init();
    temp->mName = "Monkey";
    temp->mMatrix.scale(0.5f);
    temp->mMatrix.translate(3.f, 2.f, -2.f);
    mVisualObjects.push_back(temp);

//     testing objmesh class - many of them!
    // here we see the need for resource management!
//    int x{0};
//    int y{0};
//    int numberOfObjs{100};
//    for (int i{0}; i < numberOfObjs; i++)
//    {
//        temp = new ObjMesh("../INNgine2019/Assets/monkey.obj");
//        temp->setShader(mShaderProgram[0]);
//        temp->init();
//        x++;
//        temp->mMatrix.translate(0.f + x, 0.f, -2.f - y);
//        temp->mMatrix.scale(0.5f);
//        mVisualObjects.push_back(temp);
//        if(x%10 == 0)
//        {
//            x = 0;
//            y++;
//        }
//    }

    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(gsl::Vector3D(1.f, 1.f, 4.4f));
//    mCurrentCamera->yaw(45.f);
//    mCurrentCamera->pitch(5.f);

    //new system - shader sends uniforms so needs to get the view and projection matrixes from camera
    mShaderProgram[0]->setCurrentCamera(mCurrentCamera);
    mShaderProgram[1]->setCurrentCamera(mCurrentCamera);
    mShaderProgram[2]->setCurrentCamera(mCurrentCamera);
}

///Called each frame - doing the rendering
void Renderer::render(double deltaTime)
{
    if(isExposed())
    {
        mCurrentCamera->update(deltaTime);

        mContext->makeCurrent(this);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto visObject: mVisualObjects)
        {
            visObject->draw();
    //        checkForGLerrors();
        }

        checkForGLerrors();

        mContext->swapBuffers(this);
    }
}

void Renderer::setupPlainShader(int shaderIndex)
{
    mMatrixUniform0 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "mMatrix" );
    vMatrixUniform0 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "vMatrix" );
    pMatrixUniform0 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "pMatrix" );
}

void Renderer::setupTextureShader(int shaderIndex)
{
    mMatrixUniform1 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "mMatrix" );
    vMatrixUniform1 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "vMatrix" );
    pMatrixUniform1 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "pMatrix" );
    mTextureUniform = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "textureSampler");
}


void Renderer::exposeEvent(QExposeEvent *)
{
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
        if(mInput.W)
            mLight->mMatrix.translateZ(-mCameraSpeed);
        if(mInput.S)
            mLight->mMatrix.translateZ(mCameraSpeed);
        if(mInput.D)
            mLight->mMatrix.translateX(mCameraSpeed);
        if(mInput.A)
            mLight->mMatrix.translateX(-mCameraSpeed);
        if(mInput.Q)
            mLight->mMatrix.translateY(mCameraSpeed);
        if(mInput.E)
            mLight->mMatrix.translateY(-mCameraSpeed);
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
