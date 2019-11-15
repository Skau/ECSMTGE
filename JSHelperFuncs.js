// Helper functions used in JS. These are automatically added to the top of all script files. Put stuff here you want all scripts to have.


function exists(comp)
{
    for(let i = 0; i < accessedComponents.length; ++i) {
        if (accessedComponents[i].ID == comp.ID && accessedComponents[i].ComponentType == comp.ComponentType) {
            return true;
        }
    }
    return false;
}

function addComponent(name, id = 0)
{
    const comp = me._addComponent(name, id);
    if(comp != null) {
        if (!exists(comp)) {
            accessedComponents.push(comp);
        }
    }
    return comp;
}

function getComponent(name, id = 0)
{
    const comp = me._getComponent(name, id);
    if(comp != null) {
        if(!exists(comp)) {
            accessedComponents.push(comp);
        }
    }
    return comp;
}

function destringify(obj)
{
    return Function('"use strict"; return (' + obj + ')')();
}

function deleteUnusedVariables(args)
{
    for (i = accessedComponents.length - 1; 0 < i; --i)
    {
        let removeComp = true;
        for (argIndex in args)
        {
            let variable = destringify(args[argIndex]);

            if (variable === undefined)
                continue;

            if (variable.ComponentType === accessedComponents[i].ComponentType && variable.ID === accessedComponents[i].ID)
            {
                removeComp = false;
                break;
            }
        }

        if (removeComp)
        {
            accessedComponents.splice(i, 1);
        }
    }
}
