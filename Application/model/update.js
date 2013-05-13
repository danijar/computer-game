
// moving objects
position(spark, 3 * sine(5, 0.0), 1.5 * sine(3, 0.25) + 3, 3 * sine(10, 0.5));
position(blue, 30 * sine(5, 0.0), 5, 30 * sine(5, 0.25));
position(red, 15, 5 * sine(7, 0, true) + 3, 18);
rotation(cube, 360 * sine(0.005, 0.0, true), 360 * sine(0.007, 0.0, true), 360 * sine(0.009, 0.0, true));
position(cube, 3 * sine(0.3), 8, 3 * sine(0.3, 0.25));

// helper functions
function sine(speed, offset, positive) {
    speed = speed || 1;
    offset = offset || 0;
    positive = positive || false;
    var value = Math.sin((speed * time() / 2500) + 2 * Math.PI * offset);
    if (positive == true) value = (value + 1) / 2;
    return value;
}
function time() {
    var data = new Date();
    return data.getTime();
}
