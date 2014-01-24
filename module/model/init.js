
// sun
light(0.5, 1.0, 1.5, 0, 0.75, 0.74, 0.67, 0.8, 'directional');
light(0.5, 1.0,-1.0, 0, 0.75, 0.74, 0.67, 0.4, 'directional');

// strong point light for debugging
// light(0, 8, 0, 100, .2, .4, .9, 3.0, 'point');

// helper function
function cook(from, to) { return (parseFloat(10000 * Math.random() % parseInt((to - from) * 10)) / 10) + from; };

// shoot cube
on('InputBindShoot', function() {
	var id = model('cube.prim', 'magic.mtl', 0, 10, 0, 0, 0, 0, 0.25, 5);
	var origin = position(camera());
	var lookat = direction(camera());
	rotation(id, 360 * cook(0, 6.28), 360 * cook(0, 6.28), 360 * cook(0, 6.28));
	position(id, origin[0] + lookat[0], origin[1] + lookat[1], origin[2] + lookat[2]);
	impulse(id, 200 * lookat[0], 200 * lookat[1], 200 * lookat[2]);

	barrel();
});

function barrel() {
	var origin = position(camera());
	var lookat = direction(camera());
	var id = model('barrel.3ds', 'barrel.mtl', 0, 10, 0, 0, 0, 0, 1, 0);
	rotation(id, 360 * cook(0, 6.28), 360 * cook(0, 6.28), 360 * cook(0, 6.28));
	var distance = 2;
	position(id, origin[0] + distance * lookat[0], origin[1] + distance * lookat[1], origin[2] + distance * lookat[2]);
}
