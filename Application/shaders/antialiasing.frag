#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform vec2 frameBufSize;

void depth(out float value, in vec2 offset)
{
    value = texture2D(position_tex, coord + offset / frameBufSize).z / 1000.0f;
}

void normal(out vec3 value, in vec2 offset)
{
    value = texture2D(normal_tex, coord + offset / frameBufSize).xyz;
}

void main()
{
    // depth

    float dc, dn, ds, de, dw;
    depth(dc, vec2( 0,  0));
    depth(dn, vec2( 0, +1));
    depth(ds, vec2( 0, -1));
    depth(de, vec2(+1,  0));
    depth(dw, vec2(-1,  0));
    
    float dvertical   = abs(dc - ((dn + ds) / 2));
    float dhorizontal = abs(dc - ((de + dw) / 2));
    float damount = 1000 * (dvertical + dhorizontal);

    // normals

    vec3 nc, nn, ns, ne, nw;
    normal(nc, vec2( 0,  0));
    normal(nn, vec2( 0, +1));
    normal(ns, vec2( 0, -1));
    normal(ne, vec2(+1,  0));
    normal(nw, vec2(-1,  0));

    float nvertical   = dot(vec3(1), abs(nc - ((nn + ns) / 2.0)));
    float nhorizontal = dot(vec3(1), abs(nc - ((ne + nw) / 2.0)));
    float namount = 50 * (nvertical + nhorizontal);

    // blur

    const int radius = 1;
    vec3 blur = vec3(0);
    int n = 0;
    for(float u = -radius; u <= +radius; ++u)
    for(float v = -radius; v <= +radius; ++v)
    {
        blur += texture2D(image_tex, coord + vec2(u, v) / frameBufSize).rgb;
        n++;
    }
    blur /= n;

    // result

    float amount = mix(damount, namount, 0.5);
    vec3 color = texture2D(image_tex, coord).rgb;
    image = vec4(mix(color, blur, min(amount, 0.75)), 1.0);
}
