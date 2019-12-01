var lifeTime = 2;
var time = 0;

var collider;

// This will be run once when play button is pressed
function beginPlay()
{
	collider = addComponent("collider");
	collider.CollisionType = 1;
}

// This will be run once every frame
function tick()
{
	time += engine.deltaTime;
	if(time >= lifeTime)
	{
		engine.destroyEntity(me.ID);
	}
	//console.log("Tick called on entity " + me.ID);
}

// This will be run once when stop button is pressed
function endPlay()
{
	//console.log("End play called on entity " + me.ID);
}

// This will be run when input is pressed
// NOTE:
// This requires that the entity has an Input component
// and control while playing is true
function inputPressed(inputs)
{
	//for (let i = 0; i < inputs.length; i++)
	//{
		//console.log(inputs[i] + " pressed");
	//}
}

// This will be run when input is relased
// NOTE:
// This requires that the entity has an Input component
// and control while playing is true
function inputReleased(inputs)
{
	//for (let i = 0; i < inputs.length; i++)
	//{
		//console.log(inputs[i] + " released");
	//}
}

function mouseMoved(offset)
{
	//console.log("Mouse offset: x: " + offset[0] + ", y: " + offset[1]);
}

// This will be run when collision with another entity occurs
function onHit(hitInfo)
{
	engine.destroyEntity(hitInfo.ID);
	engine.destroyEntity(me.ID);
}
