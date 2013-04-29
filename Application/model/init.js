
// bottom
model("plane.prim", "chess.mtl", 0, 0, 0, 0, 0, 0, 1, 0);

// qubes
var a = 3;
for (x = -3; x <= 3; ++x)
    for (z = -3; z <= 3; ++z)
        model("qube.prim", "dirt.mtl", a * x, 1, a * z, 0, 0, 0, 1, 0);
for (x = -3; x <= 3; x += 3)
    for (z = -3; z <= 3; z += 3)
        if (x == 0 && z == 0) continue;
        else model("qube.prim", "grass.mtl", a * x, a + 1, a * z, 0, 0, 0, 1, 0);

// models
model('qube.prim', 'magic.mtl', 20, 5, -6, 27, 31, 57, 1, 1.0);
model("shrine.3ds", "shrine.mtl", 40, 0, -10, -90, 0, -30, 2.5, 0);
model("rock.obj", "rock.mtl", 27, 2, 23, -90, 0, 0, 5.0, 0);
model("barrel.3ds", "barrel.mtl", 16, 0, 8, -90, 0, 0, 4.0, 0);

var barrel = model("barrel.3ds", "barrel.mtl", 16, 0, 8, -90, 0, 0, 4.0, 0);
var position = getposition(barrel);
print("position of barrel is " + position[0] + ", " + position[1] + ", " + position[2]);

// lights
light(-10, 15, -15, 100, 1.0, 1.0, 1.0, 1.0, true); // white
light(-10, 15, 15, 80, 0.2, 1.0, 0.2, 2.0, true); // green
light(40, 10, -10, 100, 1.0, 1.0, 1.0, 2.5, true); // white
light(20, 6, -5, 25, 1.0, 0.6, 0.2, 1.0, true); // brown

// moving objects
var spark = light(0, 5, 0, 20, 1.0, 1.0, 1.0, 3.0, false);
var blue = light(50, 20, 50, 120, 0.0, 0.4, 1.0, 5.0, false);
var red = light(30, 8, 36, 30, 1.0, 0.0, 0.0, 7.0, false);
