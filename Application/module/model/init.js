
// sun
light(0.5, 1.0, 1.5, 0, 0.75, 0.74, 0.67, 0.8, 'directional');
light(0.5, 1.0,-1.0, 0, 0.75, 0.74, 0.67, 0.4, 'directional');

// helper function
function cook(from, to) { return (parseFloat(10000 * Math.random() % parseInt((to - from) * 10)) / 10) + from; };

// shoot cube
on('InputBindShoot', function(){
	var id = model('qube.prim', 'magic.mtl', 0, 10, 0, 0, 0, 0, 0.5, 5);
	var origin = position(camera());
	var lookat = direction(camera());
	rotation(id, 360 * cook(0, 6.28), 360 * cook(0, 6.28), 360 * cook(0, 6.28));
	position(id, origin[0] + lookat[0], origin[1] + lookat[1], origin[2] + lookat[2]);
	impulse(id, 200 * lookat[0], 200 * lookat[1], 200 * lookat[2]);
});
