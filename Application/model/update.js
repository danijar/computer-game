
// moving objects
setposition(spark, 7 * sine(5, 0.0), 3 * sine(3, 0.25) + 3, 7 * sine(10, 0.5));
setposition(blue, 55 * sine(5, 0.0), 10, 55 * sine(5, 0.25));
setposition(red, 30, 10 * sine(7, 0, true) + 7, 36);


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
