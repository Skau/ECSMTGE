#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QObject>
#include <QPoint>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class Renderer;

class InputHandler : public QObject
{
    Q_OBJECT

public:
    InputHandler(Renderer* renderer);

    static std::map<int, bool> Keys;
    static QPoint MouseOffset;

    void updateMouse();

signals:
    void escapeKeyPressed();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Renderer* mRenderer;

    QPoint mCenter, mLastPos;

};

#endif // INPUTHANDLER_H
