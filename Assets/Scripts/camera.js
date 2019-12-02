
var transform;
var camera;

var moveSpeed = 5;

var fireRate = 0.25;
var fireTime = 0;
const projectileSpeed = 8.0;
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
function tick()
{
	//console.log("Tick called on entity " + me.ID);
	if(!canFire)
	{
		fireTime += engine.deltaTime;
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

	let moved = false;
	for (let i = 0; i < inputs.length; i++)
	{
		// let rotation = new Quat(transform.Rotation[0], transform.Rotation[1], transform.Rotation[2], transform.Rotation[3]);
		// console.log("Rotation: " + rotation.s + ", " + rotation.i + ", " + rotation.j + ", " + rotation.k);
		// console.log("Position: " + transform.Position[0] + ", " + transform.Position[1] + ", " + transform.Position[2]);
		if(inputs[i] == "w")
		{
			pos = pos.sub(fwd.mult(moveSpeed * engine.deltaTime));
			moved = true;
		}
		else if(inputs[i] == "s")
		{
			pos = pos.add(fwd.mult(moveSpeed * engine.deltaTime));
			moved = true;
		}
		else if(inputs[i] == "a")
		{
			pos = pos.sub(right.mult(moveSpeed * engine.deltaTime));
			moved = true;
		}
		else if(inputs[i] == "d")
		{
			pos = pos.add(right.mult(moveSpeed * engine.deltaTime));
			moved = true;
		}
		else if(inputs[i] == "e")
		{
			pos = pos.add(up.mult(moveSpeed * engine.deltaTime));
			moved = true;
		}
		else if(inputs[i] == "q")
		{
			pos = pos.sub(up.mult(moveSpeed * engine.deltaTime));
			moved = true;
		}
		else if(inputs[i] == "mouseLeft")
		{
			if(canFire)
			{
				playSound();

				let entity = engine.spawnEntity();
				let meshComp = entity.addComponent("mesh");
				meshComp.IsVisible = true;
				meshComp.MeshData.Name = "box2";
				meshComp.MaterialData = {
					Shader: "singleColor",
					Parameters: [
						{
							"p_color": [0.0, 2.0, 0.65]
						}
					]
				};

 				let scriptComp = entity.addComponent("script");
				scriptComp.FilePath = "projectile.js";

				let transformComp = entity.addComponent("transform");

				let newPos = pos;
				newPos = newPos.sub(fwd.mult(1.5));

				transformComp.Position[0] = newPos.x;
				transformComp.Position[1] = newPos.y;
				transformComp.Position[2] = newPos.z;

				transformComp.Scale = [0.25, 0.25, 0.25];

				// transformComp.Rotation = transform.Rotation;

				let physics = entity.addComponent("physics");

				let vel = fwd.arr();
				for (i in vel)
					vel[i] = vel[i] * -projectileSpeed;

				physics.Velocity = vel;

				let lightComp = entity.addComponent("pointLight");
				lightComp.Intensity = 2;
				lightComp.Radius = 100;
				lightComp.Color = [0, 1.0, 0.65];

				canFire = false;
			}
		}
	}

	if(moved == true)
	{
		transform.Position[0] = pos.x;
		transform.Position[1] = pos.y;
		transform.Position[2] = pos.z;
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
