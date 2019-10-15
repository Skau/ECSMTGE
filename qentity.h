#ifndef QENTITY_H
#define QENTITY_H

#include <QObject>
#include "entitymanager.h"
#include "componentdata.h"

class QEntity : public QObject
{
    Q_OBJECT

public:
    QEntity(unsigned int _ID, QJSEngine* engine, EntityManager* _entityManager, QObject* parent = nullptr);

    Q_PROPERTY(unsigned int ID MEMBER mID)

    void updateComponents(std::vector<QJsonObject> objects);

public slots:
    QJSValue getComponent(const QString& name);


private:
    EntityManager* entityManager;
    QJSEngine* mEngine;
    unsigned int mID;
};

#endif // QENTITY_H
