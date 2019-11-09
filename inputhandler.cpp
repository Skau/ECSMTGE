#include "inputhandler.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

#include "renderer.h"

std::map<int, bool> InputHandler::Keys = {};
QPoint InputHandler::MouseOffset = {};


InputHandler::InputHandler(Renderer *renderer) : mRenderer(renderer)
{
    // Calculates the middle of the window (used for setting the mouse position)
    mCenter.setX(static_cast<int>(mRenderer->width() / 2.0f));
    mCenter.setY(static_cast<int>(mRenderer->height() / 2.0f));

    // Positions the cursor in the middle
    QCursor::setPos(mRenderer->mapToGlobal(mCenter));
}

void InputHandler::updateMouse()
{
    if(Keys[Qt::MouseButton::RightButton] == true)
    {
        mRenderer->setCursor(Qt::BlankCursor);

        QPoint mousePos = mRenderer->mapFromGlobal(QCursor::pos());

        MouseOffset.setX(mousePos.x() - mCenter.x());
        MouseOffset.setY(mousePos.y() - mCenter.y());

        QCursor::setPos(mRenderer->mapToGlobal(mCenter));
    }
    else
    {
        mRenderer->setCursor(Qt::ArrowCursor);
    }
}

// Overrided function that taps into RenderWindow and receieves all events before QT does.
// To keep events from being propagated further into QT return true to mark event as handled.
bool InputHandler::eventFilter(QObject* obj, QEvent* event)
{
    switch(event->type())
    {
    case QEvent::MouseButtonPress:
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        Keys[static_cast<int>(mouseEvent->button())] = true;
        QString string;
        if (mouseEvent->button() == Qt::MouseButton::LeftButton)
        {
            mousePress();
            string = "mouseLeft";
        }
        else if(mouseEvent->button() == Qt::MouseButton::RightButton)
        {
            string = "mouseRight";
        }

        if(string.size() && std::find(inputPressedStrings.begin(), inputPressedStrings.end(), string) == inputPressedStrings.end())
        {
            inputPressedStrings.push_back(string);
        }

        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        QString string;
        if (mouseEvent->button() == Qt::MouseButton::LeftButton)
        {
            string = "mouseLeft";
        }
        else if(mouseEvent->button() == Qt::MouseButton::RightButton)
        {
            string = "mouseRight";
        }

        if(string.size())
        {
            inputReleasedStrings.push_back(string);
            inputPressedStrings.erase(std::remove(inputPressedStrings.begin(), inputPressedStrings.end(), string), inputPressedStrings.end());
        }

        Keys[static_cast<int>(mouseEvent->button())] = false;
        return true;
    }
    case QEvent::KeyPress:
    {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        QString string;
        switch (keyEvent->key())
        {
        case Qt::Key_Escape:
        {
            escapeKeyPressed();
            string = "escape";
            break;
        }
        case Qt::Key_Shift:
        {
            string = "shift";
            break;
        }
        case Qt::Key_Control:
        {
            string = "control";
            break;
        }
        case Qt::Key_Alt:
        {
            string = "alt";
            break;
        }
        case Qt::Key_Space:
        {
            string = "space";
            break;
        }
        }

        if (!string.size())
        {
            string = keyEvent->text();
        }

        if(std::find(inputPressedStrings.begin(), inputPressedStrings.end(), string) == inputPressedStrings.end())
        {
            inputPressedStrings.push_back(string);
        }

        Keys[keyEvent->key()] = true;

        return true;
    }
    case QEvent::KeyRelease:
    {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->isAutoRepeat())
            return QObject::eventFilter(obj, event);

        QString string;
        switch (keyEvent->key())
        {
        case Qt::Key_Escape:
        {
            escapeKeyPressed();
            string = "escape";
            break;
        }
        case Qt::Key_Shift:
        {
            string = "shift";
            break;
        }
        case Qt::Key_Control:
        {
            string = "control";
            break;
        }
        case Qt::Key_Alt:
        {
            string = "alt";
            break;
        }
        case Qt::Key_Space:
        {
            string = "space";
            break;
        }
        }

        if (!string.size())
        {
            string = keyEvent->text();
        }

        inputReleasedStrings.push_back(string);
        inputPressedStrings.erase(std::remove(inputPressedStrings.begin(), inputPressedStrings.end(), string), inputPressedStrings.end());

        Keys[keyEvent->key()] = false;

        return true;
    }
    case QEvent::Wheel:
    {
        break;
    }
    case QEvent::Resize:
    {
        mCenter.setX(static_cast<int>(mRenderer->width() / 2.0f));
        mCenter.setY(static_cast<int>(mRenderer->height() / 2.0f));
        break;
    }
    // For all other event types - pass it on to QTs other systems, because we don't care about the rest at the moment
    default:
        return QObject::eventFilter(obj, event);
    }

    return false;
}
