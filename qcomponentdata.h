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

#endif // QCOMPONENTDATA_H
