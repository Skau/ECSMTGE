#include "eventhandler.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

EventHandler::EventHandler()
{

}

// Overrided function that taps into RenderWindow and receieves all events before QT does.
// To keep events from being propagated further into QT return true to mark event as handled.
bool EventHandler::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::MouseButtonPress:
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        mousePressEvent(mouseEvent);
        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        mouseReleaseEvent(mouseEvent);
        return true;
    }
    case QEvent::KeyPress:
    {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        keyPressEvent(keyEvent);
        return true;
    }
    case QEvent::KeyRelease:
    {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        keyReleaseEvent(keyEvent);
        return true;
    }
    case QEvent::Wheel:
    {
        auto wEvent = static_cast<QWheelEvent*>(event);
        wheelEvent(wEvent);
        return true;
    }
    case QEvent::Resize:
    {
        qDebug() << "Resized";
        break;
    }
    default:
        return false;
    }

    return false;
}

void EventHandler::keyPressEvent(QKeyEvent* event)
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

void EventHandler::keyReleaseEvent(QKeyEvent *event)
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

void EventHandler::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void EventHandler::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void EventHandler::mouseMoveEvent(QMouseEvent *event)
{
    if (mInput.RMB)
    {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
//        mMouseXlast = event->pos().x() - mMouseXlast;
//        mMouseYlast = event->pos().y() - mMouseYlast;

//        if (mMouseXlast != 0)
//            mCurrentCamera->yaw(mCameraRotateSpeed * mMouseXlast);
//        if (mMouseYlast != 0)
//            mCurrentCamera->pitch(mCameraRotateSpeed * mMouseYlast);
    }
//    mMouseXlast = event->pos().x();
//    mMouseYlast = event->pos().y();
}

void EventHandler::wheelEvent(QWheelEvent *event)
{
//    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput.RMB)
    {
//        if (numDegrees.y() < 1)
//            setCameraSpeed(0.001f);
//        if (numDegrees.y() > 1)
//            setCameraSpeed(-0.001f);
    }
    event->accept();
}
