uniform sampler2D texture;
uniform vec2 pos_mouse;
uniform float radius;
uniform vec3 back_color;
uniform float alpha;


void main() {
    vec2 pix_coord = vec2(gl_FragCoord);
    vec4 pixel_color = gl_Color;
    vec4 col_computed = vec4(back_color.xyz, alpha);

    float dist = sqrt(
        (pix_coord.x - pos_mouse.x) * (pix_coord.x - pos_mouse.x)
        + (pix_coord.y - pos_mouse.y) * (pix_coord.y - pos_mouse.y)
    );

    if (dist >= radius) {
        float f = dist;
        pixel_color = (20 / f) * col_computed ;
    }

    vec2 pos =  gl_TexCoord[0].xy;
    vec4 textureVers4D = texture2D(texture, pos);
    gl_FragColor = textureVers4D * pixel_color;
}