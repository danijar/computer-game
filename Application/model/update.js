
// moving light

setposition(spark, 7 * sine(5, 0.0), 3 * sine(3, 0.25) + 3, 7 * sine(10, 0.5));
setposition(blue, 55 * sine(5, 0.0), 10, 55 * sine(5, 0.25));


// helper functions

function sine(speed, offset) {
    speed = speed || 1;
    offset = offset || 0;
    return Math.sin((speed * time() / 2500) + 2 * Math.PI * offset);
}

function time() {
    var data = new Date();
    return data.getTime();
}
