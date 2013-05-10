
// bottom
model("plane.prim", "chess.mtl", 0, 0, 0, 0, 0, 0, 1, 0);

// qubes
radius = 4;
for (X = -radius; X <= radius; ++X)
for (Z = -radius; Z <= radius; ++Z)
if(X * X + Z * Z <= radius * radius)
{
	scale = 0.5 + Math.random() / 3;
	x     = 2 * (Math.random() - 0.5) * 2 + 2 * X;
	z     = 2 * (Math.random() - 0.5) * 2 + 2 * Z;
	model("qube.prim", "dirt.mtl", x, scale, z, 0, 0, 0, scale, 0);
}
for (i = 0; i < 30; ++i)
{
	scale = 0.5 + Math.random() / 3;
	x     = 5 * (Math.random() - 0.5) * 2;
	z     = 5 * (Math.random() - 0.5) * 2;
	model("qube.prim", "grass.mtl", x, 2 * (scale + 1.0) + 1.0, z, 0, 0, 0, scale, 0);
}

// models
model('qube.prim', 'magic.mtl', 10, 4, -3, 27, 31, 57, 0.5, 1.0);
model("shrine.3ds", "shrine.mtl", 20, 0, -5, -90, 0, -30, 1, 0);
model("rock.obj", "rock.mtl", 13, 1, 12, -90, 0, 0, 3, 0);
model("barrel.3ds", "barrel.mtl", 8, 0, 4, -90, 0, 0, 2, 0);

var cube = model("qube.prim", "rock.mtl", 0, 8, 0, 0, 0, 0, 1.5, 0);
var position = getposition(cube);
print("position of rotating cube is " + position[0] + ", " + position[1] + ", " + position[2]);

// lights
light(-5, 8, -8, 50, 1.0, 1.0, 1.0, 1.0, true); // white
light(-5, 8, 8, 40, 0.2, 1.0, 0.2, 2.0, true); // green
light(20, 5, -5, 50, 1.0, 1.0, 1.0, 2.5, true); // white

// moving objects
var spark = light(0, 3, 0, 10, 1.0, 1.0, 1.0, 3.0, false);
var blue = light(25, 10, 25, 60, 0.0, 0.4, 1.0, 5.0, false);
var red = light(15, 4, 18, 15, 1.0, 0.0, 0.0, 7.0, false);
