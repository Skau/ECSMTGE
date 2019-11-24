#ifndef QENTITY_H
#define QENTITY_H

#include <QObject>
#include <QJSValue>

class EntityManager;
class QJSEngine;

/**
 * @brief The wrapper used to reference itself in scripts.
 * To access this wrapper use 'me'.
 * Example: me.setMesh("suzanne");
 */
class QEntity : public QObject
{
    Q_OBJECT

public:
    QEntity(unsigned int _ID,  QObject* parent = nullptr);

    Q_PROPERTY(unsigned int ID MEMBER mID)

public slots:
    QJSValue getComponent(const QString& name);
    QJSValue addComponent(const QString& name);

private:
    unsigned int mID;

};

#endif // QENTITY_H
