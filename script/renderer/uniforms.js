

function uniforms() {
    var projection_matrix = projection();
    uniform("forms", "projection", projection_matrix);
    uniform("sky", "projection", projection_matrix);

    var frame_size = windowsize();
    uniform("edge", "frame_size", frame_size);
    uniform("occlusion", "frame_size", frame_size.map(function(x){ return 0.75 * x; }));
    uniform("blur_u", "frame_size", frame_size);
    uniform("blur_v", "frame_size", frame_size);
}

uniforms();
on("WindowRecreated", uniforms);
on("WindowResize", uniforms);
on("ShaderUpdated", uniforms);
