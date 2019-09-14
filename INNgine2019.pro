QT          += core gui widgets

TEMPLATE    = app
CONFIG      += c++17

TARGET      = INNgine2019

PRECOMPILED_HEADER = innpch.h

INCLUDEPATH +=  ./GSL
INCLUDEPATH += ./include

mac {
    LIBS += -framework OpenAL
}

win32 {
    INCLUDEPATH += $(OPENAL_HOME)\\include\\AL
    LIBS *= $(OPENAL_HOME)\\libs\\Win64\\libOpenAL32.dll.a
}

HEADERS += \
    GSL/math.h \
    GSL/quaternion.h \
    Widgets/componentwidget.h \
    Widgets/inputwidget.h \
    Widgets/meshwidget.h \
    Widgets/physicswidget.h \
    Widgets/transformwidget.h \
    app.h \
    camerasystem.h \
    componentdata.h \
    entitymanager.h \
    inputhandler.h \
    inputsystem.h \
    mainwindow.h \
    constants.h \
    gltypes.h \
    GSL/matrix2x2.h \
    GSL/matrix3x3.h \
    GSL/matrix4x4.h \
    GSL/vector2d.h \
    GSL/vector3d.h \
    GSL/vector4d.h \
    GSL/math_constants.h \
    GSL/mathfwd.h \
    Shaders/shader.h \
    Renderables/triangle.h \
    Renderables/billboard.h \
    Renderables/vertex.h \
    Renderables/xyz.h \
    Renderables/visualobject.h \
    Renderables/octahedronball.h \
    Renderables/billboard.h \
    Renderables/light.h \
    Renderables/objmesh.h \
    Renderables/skybox.h \
    Renderables/octahedronball.h \
    Renderables/trianglesurface.h \
    openalmanager.h \
    physicssystem.h \
    renderer.h \
    resourcemanager.h \
    scene.h \
    soundlistener.h \
    soundsource.h \
    texture.h \
    wavfilehandler.h \
    world.h \
    meshdata.h


SOURCES += \
    GSL/math.cpp \
    GSL/quaternion.cpp \
    Widgets/componentwidget.cpp \
    Widgets/inputwidget.cpp \
    Widgets/meshwidget.cpp \
    Widgets/physicswidget.cpp \
    Widgets/transformwidget.cpp \
    app.cpp \
    camerasystem.cpp \
    inputhandler.cpp \
    inputsystem.cpp \
    main.cpp \
    mainwindow.cpp \
    GSL/matrix2x2.cpp \
    GSL/matrix3x3.cpp \
    GSL/matrix4x4.cpp \
    GSL/vector2d.cpp \
    GSL/vector3d.cpp \
    GSL/vector4d.cpp \
    Shaders/shader.cpp \
    Renderables/billboard.cpp \
    Renderables/vertex.cpp \
    Renderables/visualobject.cpp \
    Renderables/octahedronball.cpp \
    Renderables/xyz.cpp \
    Renderables/trianglesurface.cpp \
    Renderables/light.cpp \
    Renderables/objmesh.cpp \
    Renderables/skybox.cpp \
    Renderables/triangle.cpp \
    openalmanager.cpp \
    physicssystem.cpp \
    renderer.cpp \
    resourcemanager.cpp \
    scene.cpp \
    soundlistener.cpp \
    soundsource.cpp \
    texture.cpp \
    wavfilehandler.cpp \
    world.cpp


FORMS += \
    input.ui \
    mainwindow.ui \
    mesh.ui \
    physics.ui \
    transform.ui


DISTFILES += \
    Shaders/phongshader.frag \
    Shaders/phongshader.vert \
    Shaders/plainshader.frag \
    Shaders/plainshader.vert \
    Shaders/textureshader.frag \
    Shaders/textureshader.vert \
    GSL/README.md \
    README.md
