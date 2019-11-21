#include <QApplication>
#include <QSplashScreen>

#include "app.h"

int main(int argc, char *argv[])
{
    //Forces the usage of desktop OpenGL
    //Attribute must be set before Q(Gui)Application is constructed:
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    //Makes an Qt application
    QApplication a(argc, argv);

    QSplashScreen *mSplash = new QSplashScreen;
    mSplash->setPixmap(QPixmap("../INNgine2019/Assets/Icons/ECSMTGE.png")); // splash picture
    mSplash->show();

    // App is hub, app is life
    App app;

    mSplash->hide();

    return a.exec();
}
