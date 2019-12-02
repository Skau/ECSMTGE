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
    /**
     * @brief Get the component with the given name. If it doesn't exist the return value is undefined.
     */
    QJSValue getComponent(const QString& name);
    /**
     * @brief Adds a component with the given name. Returns the component. If it already is added the current one will be returned instead.
     */
    QJSValue addComponent(const QString& name);
    /**
     * @brief Requires a sound component with a sound setup through the editor.
     */
    void playSound();

private:
    unsigned int mID;

};

#endif // QENTITY_H
