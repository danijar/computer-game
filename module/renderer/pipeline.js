
rendertarget("position");
rendertarget("normal");
rendertarget("albedo");
rendertarget("specular");
rendertarget("depth", "DEPTH24_STENCIL8");

renderpass("forms", {
    vertex: "forms.vert",
    fragment: "forms.frag",
    targets: {
        "COLOR_ATTACHMENT0"        : "position",
        "COLOR_ATTACHMENT1"        : "normal",
        "COLOR_ATTACHMENT2"        : "albedo",
        "COLOR_ATTACHMENT3"        : "specular",
        "DEPTH_STENCIL_ATTACHMENT" : "depth",
    },
    draw: "FORMS",
    depth: true,
    stencil: ["ALWAYS", 1, "REPLACE"],
});

renderpass("sky", {
    vertex: "forms.vert",
    fragment: "forms.frag",
    targets: {
        "COLOR_ATTACHMENT0"        : "position",
        "COLOR_ATTACHMENT1"        : "normal",
        "COLOR_ATTACHMENT2"        : "albedo",
        "DEPTH_STENCIL_ATTACHMENT" : "depth",
    },
    fallbacks: {
        "albedo": [1, 1, 1],
    },
    draw: "SKY",
    clear: false,
    stencil: ["EQUAL", 0],
});

rendertarget("light");

renderpass("lights", {
    fragment: "light.frag",
    targets: { "COLOR_ATTACHMENT0": "light" },
    samplers: { "positions": "position", "normals": "normal", "speculars": "specular" },
    fallbacks: { "light": [.5, .5, .5] },
    draw: "LIGHTS",
    stencil: ["GEQUAL", 1],
});

rendertarget("edge");

renderpass("edge", {
    fragment: "edge.frag",
    targets: { "COLOR_ATTACHMENT0": "edge" },
    samplers: { "depth_tex": "depth", "normal_tex": "normal" },
    fallbacks: { "edge": [0, 0, 0] },
});

rendertarget("image");

renderpass("combine", {
    fragment: "combine.frag",
    targets: { "COLOR_ATTACHMENT0": "image" },
    samplers: {
        "albedo": "albedo",
        "lights": "light",
        "depth": "depth",
    },
});

rendertarget("occlusion", "RGB16F", 0.75);

rendertarget("noise", "RGBA16", 0.0);
rendertargetload("noise", "noise.png", true, false, false);

renderpass("occlusion", {
    fragment: "occlusion.frag",
    targets: { "COLOR_ATTACHMENT0": "occlusion" },
    samplers: {
        "depth_tex": "depth",
        "normal_tex": "normal",
        "noise_tex": "noise",
    },
    fallbacks: { "occlusion": [1, 1, 1] },
    size: 0.75,
    stencil: ["GEQUAL", 1],
});

rendertarget("occluded");

renderpass("apply", {
    fragment: "apply.frag",
    targets: { "COLOR_ATTACHMENT0": "occluded" },
    samplers: {
        "image_tex": "image",
        "effect_tex": "occlusion",
        "noise_tex": "noise",
    },
});

rendertarget("result");

renderpass("fog", {
    fragment: "fog.frag",
    targets: { "COLOR_ATTACHMENT0": "result" },
    samplers: {
        "colors": "occluded",
        "depths": "depth",
        "lights": "light",
    },
    stencil: ["GEQUAL", 1],
});

rendertarget("temp");

renderpass("blur_u", {
    fragment: "blur_u.frag",
    targets: { "COLOR_ATTACHMENT0": "temp" },
    samplers: { "image_tex": "result" },
});

rendertarget("blur");

renderpass("blur_v", {
    fragment: "blur_v.frag",
    targets: { "COLOR_ATTACHMENT0": "blur" },
    samplers: { "image_tex": "temp" },
});

rendertarget("antialiasing");

renderpass("antialiasing", {
    fragment: "antialiasing.frag",
    targets: { "COLOR_ATTACHMENT0": "antialiasing" },
    samplers: {
        "image_tex": "result",
        "blur_tex": "blur",
        "edge_tex": "edge",
    },
    fallbacks: { "antialiasing": "result" },
});

rendertarget("tonemapped");

renderpass("tonemapping", {
    fragment: "tonemapping.frag",
    targets: { "COLOR_ATTACHMENT0": "tonemapped" },
    samplers: { "image_tex": "antialiasing" },
    fallbacks: { "tonemapped": "antialiasing" },
});

renderpass("screen", {
    fragment: "screen.frag",
    samplers: { "image_tex": "tonemapped" },
    draw: "SCREEN",
});

renderpass("preview", {
    fragment: "preview.frag",
    samplers: {
        "first": "normal",
    	"second": "depth",
    	"third": "light",
    	"fourth": "occlusion",
    },
    draw: "SCREEN",
    clear: false,
});
