class Vec3
{
    constructor(args, y = 0.0, z = 0.0)
    {
        if (Array.isArray(args))
        {
            y = args[1];
            z = args[2];
            args = args[0];
        }
        else if (typeof args === "object")
        {
            y = args.y;
            z = args.z;
            args = args.x;
        }

        if (typeof args !== "number")
            args = 0.0;
        if (typeof y !== "number")
            y = 0.0;
        if (typeof z !== "number")
            z = 0.0;

        this.x = args;
        this.y = y;
        this.z = z;

        // aliases
        this.norm = this.normalized;
    }

    length()
    {
        return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
    }

    normalized()
    {
        let l = this.length();
        return (l > 0.0) ? new Vec3(this.x / l, this.y / l, this.z / l) : new Vec3();
    }
}

class Quat
{
    // Constructor can be run with an object that describes the
    // object or with individual parameters
    constructor(args, i = 0.0, j = 0.0, k = 0.0)
    {
        if (Array.isArray(args))
        {
            i = args[1];
            j = args[2];
            k = args[3];
            args = args[0];
        }
        else if (typeof args === "object")
        {
            i = args.i;
            j = args.j;
            k = args.k;
            args = args.s;
        }

        if (typeof args !== "number")
            args = 1.0;
        if (typeof i !== "number")
            i = 0.0;
        if (typeof j !== "number")
            j = 0.0;
        if (typeof k !== "number")
            k = 0.0;

        this.s = args;
        this.i = i;
        this.j = j;
        this.k = k;
    }

    mult(b)
    {
        if (typeof b === "number")
        {
            return new Quat(this.s * b, this.i * b, this.j * b, this.k * b);
        }
        else if (b instanceof Quat)
        {
            return new Quat(
                this.s * b.s - this.i * b.i - this.j * b.j - this.k * b.k,
                this.s * b.i + this.i * b.s + this.j * b.k - this.k * b.j,
                this.s * b.j - this.i * b.k + this.j * b.s + this.k * b.i,
                this.s * b.k + this.i * b.j - this.j * b.i + this.k * b.s
            );
        }

        return undefined;
    }

    div(b)
    {
        if (typeof b === "number")
        {
            return new Quat(this.s / b, this.i / b, this.j / b, this.k / b);
        }

        return undefined;
    }

    conj()
    {
        return new Quat(this.s, -this.i, -this.j, -this.k);
    }

    static dot(a, b)
    {
        if (a instanceof Quat && b instanceof Quat)
        {
            return a.s * b.s + a.i * b.i + a.j * b.j + a.k * b.k;
        }

        return undefined;
    }

    inverse()
    {
        return this.conj().div(Quat.dot(this, this));
    }

    right()
    {
        return new Vec3(
            1.0 - 2.0 * this.j * this.j - 2.0 * this.k * this.k,
            2.0 * this.i * this.j + 2.0 * this.s * this.k,
            2.0 * this.i * this.k - 2.0 * this.s * this.j
        ).normalized();
    }

    up()
    {
        return new Vec3(
            2.0 * this.i * this.j - 2.0 * this.s * this.k,
            1.0 - 2.0 * this.i * this.i - 2.0 * this.k * this.k,
            2.0 * this.j * this.k + 2.0 * this.s * this.i
        ).normalized();
    }

    forward()
    {
        return new Vec3(
            2.0 * this.i * this.k + 2.0 * this.s * this.j,
            2.0 * this.j * this.k + 2.0 * this.s * this.i,
            1.0 - 2.0 * this.i * this.i - 2.0 * this.j * this.j
        ).normalized();
    }
}
