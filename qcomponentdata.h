#ifndef QCOMPONENTDATA_H
#define QCOMPONENTDATA_H

#include <QObject>
#include "componentdata.h"

struct QMeshComponent : public QObject
{
    Q_OBJECT
public:
    QMeshComponent(MeshComponent& comp) : mComp(comp) {}

public slots:
    void setMesh(const QString& name);
    void setShader(const QString& name);

private:
    MeshComponent& mComp;
};


struct QTransformComponent : public QObject
{
    Q_OBJECT
public:
    QTransformComponent(TransformComponent& comp) : mComp(comp) {}

public slots:


private:
    TransformComponent& mComp;
};

#endif // QCOMPONENTDATA_H
