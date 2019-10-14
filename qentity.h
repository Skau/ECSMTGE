#ifndef QENTITY_H
#define QENTITY_H

#include <QObject>
#include "entitymanager.h"
#include "componentdata.h"

class QEntity : public QObject
{
    Q_OBJECT

public:
    QEntity(unsigned int _ID, EntityManager* _entityManager, QObject* parent = nullptr);

    Q_PROPERTY(unsigned int ID MEMBER mID)

public slots:
    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setScale(float value);
    void setVelocity(float x, float y, float z);

    QObject* getComponent(const QString& name);

private:
    EntityManager* entityManager;
    unsigned int mID;
};

#endif // QENTITY_H
