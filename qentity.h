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
    QEntity(unsigned int _ID, QJSEngine* engine,  EntityManager* _entityManager, QObject* parent = nullptr);

    Q_PROPERTY(unsigned int ID MEMBER mID)

public slots:
    QJSValue getComponent(const QString& name);
    QJSValue addComponent(const QString& name);

    void setMesh(const QString& name);
    void setShader(const QString& name);

    // All functions prefixed with '_' should never be called directly! They are used by underlying JS code.

    QJSValue _getComponent(const QString& name, unsigned id = 0);
    QJSValue _addComponent(const QString& name, unsigned id = 0);

private:
    EntityManager* entityManager;
    QJSEngine* mEngine;
    unsigned int mID;

};

#endif // QENTITY_H
