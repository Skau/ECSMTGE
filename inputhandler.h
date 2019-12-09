#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QObject>
#include <QPoint>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class Renderer;

/**
 * @brief This class leverages the renderer for all input. It filters out all events and provides the inputsystem and Javascript with what it needs.
 */
class InputHandler : public QObject
{
    Q_OBJECT

public:
    InputHandler(Renderer* renderer);

    /**
     * @brief Map holding all current input keys with a bool if it is pressed or not. Casted int from Qt::Key
     */
    static std::map<int, bool> Keys;
    /**
     * @brief Holds the current cursor offset from the center of the render window.
     */
    static QPoint MouseOffset;

    /**
     * @brief Resets mouse to middle and updates the mouse offset. Also keeps track of then to hide the cursor based on if the game is currently playing.
     */
    void updateMouse(bool currentlyPlaying);

    /**
     * @brief Vector holding the string representation of pressed keys this frame. Used in Javascript for input events.
     */
    std::vector<QString> inputPressedStrings;
    /**
     * @brief Vector holding the string representation of released keys this frame. Used in Javascript for input events.
     */
    std::vector<QString> inputReleasedStrings;

signals:
    void escapeKeyPressed();
    void mousePress();

protected:
    /**
     * @brief Routes all events from the render window here. We only care about input events, so all others are sendt to the window.
     */
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Renderer* mRenderer;
    QPoint mCenter, mLastPos;

};

#endif // INPUTHANDLER_H
