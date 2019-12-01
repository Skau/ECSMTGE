// Global script variables
const spawnRatio = 3.0;
var timer = 0.0;

function beginPlay()
{
    timer = 0.0;
}

function tick()
{
    timer += engine.deltaTime;

    if (timer > spawnRatio)
    {
        timer -= spawnRatio;
        spawnMeteoroid();
    }
}

function spawnMeteoroid()
{
    let entity = engine.spawnEntity();
    let meshComp = entity.addComponent("mesh");
    meshComp.IsVisible = true;
    meshComp.MeshData.Name = "ball";
    meshComp.MaterialData = {
        Shader: "phong",
        Parameters: [
            {
                p_color: randomMeteoroidColor()
            }
        ]
    };

    let transComp = entity.addComponent("transform");
    let randPoint = getRandomPointInSphere([0.0, 0.0, 0.0], 10.0);
    transComp.Position = randPoint;
    // Scale is random between 1 and 5
    let randScale = (n) => {return Math.random() * (n - 1) + 1};
    transComp.Scale = [randScale(3), randScale(3), randScale(3)];

    let physics = entity.addComponent("physics");
    let randomVec = (mult) => {
        let ret = [];
        for (i = 0; i < 3; ++i)
            ret[i] = (Math.random() * 2 - 1) * mult;
        return ret;
    };
    physics.Velocity = randomVec(0.3);

    let collComp = entity.addComponent("collider");
    // collComp.CollisionType = 3;
    // collComp.Extents = findMiddleInArray(transComp.Scale);
    collComp.CollisionType = 1;
    collComp.Extents = transComp.Scale;
}

function getRandomPointInSphere(centre, radius)
{
    let randomNum = () => {
        return Math.random() * 2 - 1;
    };
    let randomVec = [randomNum(), randomNum(), randomNum()];
    let result = [];
    for (i = 0; i < 3; ++i)
        result[i] = centre[i] + randomVec[i] * radius;

    return result;
}

function findMiddleInArray(arr) {
    let smallest = 0;
    let biggest = 0;
    for (item of arr)
    {
        smallest = item < smallest ? item : smallest;
        biggest = item < biggest ? item : biggest;
    }
    return smallest + (biggest - smallest) / 2.0;
}

function randomMeteoroidColor()
{
    let ret = [];
    let greyTone = Math.random();
    for (i = 0; i < 3; ++i)
    {
        ret[i] = clamp(greyTone + ((Math.random() - 0.5) * 0.1), 0.0, 1.0);
    }
    return ret;
}

function endPlay()
{
}
