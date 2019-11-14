
var transform;
var camera;

var moveSpeed = 5;

var fireRate = 2;
var fireTime = 0;
var canFire = true;

function clamp(x, min, max)
{
	return x < min ? min : x > max ? max : x;
}

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
	if(!canFire)
	{
		fireTime += deltaTime;
		if(fireTime >= fireRate)
		{
			canFire = true;
			fireTime = 0;
		}
	}
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
	let cameraRot = new Quat(transform.Rotation);
	cameraRot = cameraRot.inverse();
	let fwd = cameraRot.forward();
	let right = cameraRot.right();
	let up = cameraRot.up();

	let pos = new Vec3(transform.Position);

	for (let i = 0; i < inputs.length; i++)
	{
		// let rotation = new Quat(transform.Rotation[0], transform.Rotation[1], transform.Rotation[2], transform.Rotation[3]);
		// console.log("Rotation: " + rotation.s + ", " + rotation.i + ", " + rotation.j + ", " + rotation.k);
		// console.log("Position: " + transform.Position[0] + ", " + transform.Position[1] + ", " + transform.Position[2]);
		if(inputs[i] == "w")
		{
			pos = pos.sub(fwd.mult(moveSpeed * engine.deltaTime));
		}
		else if(inputs[i] == "s")
		{
			pos = pos.add(fwd.mult(moveSpeed * engine.deltaTime));
		}
		else if(inputs[i] == "a")
		{
			pos = pos.sub(right.mult(moveSpeed * engine.deltaTime));
		}
		else if(inputs[i] == "d")
		{
			pos = pos.add(right.mult(moveSpeed * engine.deltaTime));
		}
		else if(inputs[i] == "e")
		{
			pos = pos.add(up.mult(moveSpeed * engine.deltaTime));
		}
		else if(inputs[i] == "q")
		{
			pos = pos.sub(up.mult(moveSpeed * engine.deltaTime));
		}
		else if(inputs[i] == "mouseLeft")
		{
			if(canFire)
			{
				let entity = engine.spawnCube();

				let scriptComp = entity.addComponent("script");
				scriptComp.FilePath = "../INNgine2019/Assets/Scripts/projectile.js";

				let transformComp = entity.getComponent("transform");
				let newPos = pos;
				newPos = newPos.sub(fwd.mult(1.5));
			
				transformComp.Position[0] = newPos.x;
				transformComp.Position[1] = newPos.y;
				transformComp.Position[2] = newPos.z;

				transformComp.Scale = [0.25, 0.25, 0.25];

				let physics = entity.addComponent("physics");

				physics.Velocity[0] = -fwd.x * 2;
				physics.Velocity[1] = -fwd.y * 2;
				physics.Velocity[2] = -fwd.z * 2;

				let collider = addComponent("collider");
				collider.CollisionType = 2;
				collider.Extents = [1, 1, 1];
				transform.ColliderBoundsOutdated = true;
			
				canFire = false;
			}
		}
	}

	transform.Position[0] = pos.x;
	transform.Position[1] = pos.y;
	transform.Position[2] = pos.z;
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
	camera.Yaw += offset[0] * 5 * engine.deltaTime;
	if (camera.Yaw > 360)
		camera.Yaw -= 360;
	if (camera.Yaw < -360)
		camera.Yaw += 360;

	camera.Pitch += offset[1] * 5 * engine.deltaTime;
	camera.Pitch = clamp(camera.Pitch, -89, 89);
}

// This will be run when collision with another entity occurs
function onHit(hitInfo)
{
	//console.log("Collided with entity ID: " + hitInfo.ID);
}
