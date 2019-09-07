#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>

#include "input.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class EventHandler : public QObject
{
    Q_OBJECT

public:
    EventHandler();

    Input getInput() { return mInput; }

signals:
    void escapeKeyPressed();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    Input mInput;
};

#endif // EVENTHANDLER_H
