#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include <QObject>
#include <QJSEngine>
#include <memory>
#include <vector>
#include "componentdata.h"

class QJSEngine;

class ScriptSystem : public QObject
{
    Q_OBJECT
public:
    static ScriptSystem* get()
    {
        static ScriptSystem instance;
        return &instance;
    }

    bool evaluate(const std::string& text, std::string& errorMessage, QJSValue& jsValue);

    void beginPlay(std::vector<ScriptComponent>& comps);
    void tick(float deltaTime, std::vector<ScriptComponent>& comps);
    void endPlay(std::vector<ScriptComponent>& comps);
    void call(QString function, QString contents, QString fileName, ScriptComponent& comp);
    void call(const std::string& function, QJSValueList params, ScriptComponent& comp);
    void call(const std::string& function, QJSValueList params, std::vector<ScriptComponent>& comps);
    void call(const std::string& function, std::vector<ScriptComponent>& comps);
private:
    ScriptSystem();
    QString checkError(QJSValue value);
    bool load(ScriptComponent comp);


    QJSEngine mJSEngine;
    QJSValue mGlobalObject;
};

#endif // SCRIPTSYSTEM_H
