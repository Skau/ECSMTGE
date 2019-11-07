// Helper functions used in JS. These are automatically added to the top of all script files. Put stuff here you want all scripts to have.

function addComponent(name, id = 0)
{
    let comp = me._addComponent(name, id);
    if(comp != null){
        accessedComponents.push(comp);
    }
    return comp;
}

function getComponent(name, id = 0)
{
    const comp = me._getComponent(name, id);
    if(comp != null){
        accessedComponents.push(comp);
        return comp;
    }
    return null;
}
