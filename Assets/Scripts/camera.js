
var transform;
var camera;

var moveSpeed = 5;

// This will be run once when play button is pressed
function beginPlay()
{
	transform = getComponent("transform");
	camera = getComponent("camera");
}

// This will be run once every frame
function tick(deltaTime)
{
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
	for (let i = 0; i < inputs.length; i++)
	{
		if(inputs[i] == "w")
		{
			transform.Position[2] -= moveSpeed * engine.deltaTime;
		}
		else if(inputs[i] == "s")
		{
			transform.Position[2] += moveSpeed * engine.deltaTime;
		}
		else if(inputs[i] == "a")
		{
			transform.Position[0] -= moveSpeed * engine.deltaTime;
		}
		else if(inputs[i] == "d")
		{
			transform.Position[0] += moveSpeed * engine.deltaTime;
		}
		else if(inputs[i] == "mouseLeft")
		{
			let entity = engine.spawnCube();

			//let transformComp = entity.getComponent("transform");
			//transformComp.Scale = [5.25, 2.25, 0.75];

			let physics = entity.addComponent("physics");
			physics.Velocity[0] += 2;
		}
	}
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
	camera.Yaw 		+= offset[0] * 5 * engine.deltaTime;
	camera.Pitch 	+= offset[1] * 5 * engine.deltaTime;
}

// This will be run when collision with another entity occurs
function onHit(hitInfo)
{
	//console.log("Collided with entity ID: " + hitInfo.ID);
}
