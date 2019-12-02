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

function addComponent(name)
{
    return me.addComponent(name);
}

function getComponent(name)
{
    return me.getComponent(name);
}

function playSound()
{
    return me.playSound();
}

function destringify(obj)
{
    return Function('"use strict"; return (' + obj + ')')();
}

function deleteUnusedVariables(args)
{
    for (let i = accessedComponents.length - 1; 0 < i; --i)
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

function updateLoop(instructions)
{
    if (!Array.isArray(instructions))
    {
        console.log("instructions isn't an array!");
        return;
    }

    // const compsBefore = accessedComponents.splice();

    for (i = 0; i < instructions.length; ++i)
    {
        if (typeof instructions[i] === "undefined")
            continue;

        if (typeof instructions[i].func === "function")
        {
            if (typeof instructions[i].params !== "undefined")
                instructions[i].func(instructions[i].params);
            else
                instructions[i].func();
        }
        else if (typeof instructions[i].func === "string")
        {
            let func = destringify(instructions[i].func);
            if (typeof instructions[i].params !== "undefined")
                func(instructions[i].params);
            else
                func();
        }
    }

    // let returnVals = [];
    //
    // for (i in accessedComponents)
    // {
    //     let changed = true;
    //     for (j in compsBefore)
    //     {
    //         if (compsBefore[j] == accessedComponents[i])
    //             changed = false;
    //     }
    //
    //     if (changed)
    //         returnVals.push(accessedComponents[i]);
    // }
    //
    // return returnVals;
}
