#ifndef QENTITY_H
#define QENTITY_H

#include <QObject>
#include <QJSValue>

class EntityManager;
class QJSEngine;

/**
 * @brief Wraps an entity for the JavaScript engine. All public slots and Q_PROPERTIES are available in JavaScript.
 */
class QEntity : public QObject
{
    Q_OBJECT

public:
    QEntity(unsigned int _ID, QJSEngine* engine, EntityManager* _entityManager, QObject* parent = nullptr);

    Q_PROPERTY(unsigned int ID MEMBER mID)

    /**
     * @brief Called by the ScriptSystem after a script has run to propagate changes to the components.
     */
    void updateComponents(std::vector<QJsonObject> objects);


    // Every public slot can be called from JavaScript.
public slots:
    /**
     * @brief Returns the component JSON object for use in JavaScript.
     */
    QJSValue getComponent(const QString& name);
    /**
     * @brief Adds the component and returns the JSON object for use in JavaScript.
     */
    QJSValue addComponent(const QString& name);


    // Other helper functions

    void setMesh(const QString& name);
    void setShader(const QString& name);

private:
    EntityManager* entityManager;
    QJSEngine* mEngine;
    unsigned int mID;
};

#endif // QENTITY_H
