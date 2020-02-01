uniform sampler2D texture;
uniform vec2 pos_mouse;
uniform float radius;
uniform vec3 back_color;
uniform float alpha;


void main() {
    vec2 pix_coord = vec2(gl_FragCoord); // 4d coord. pixel -> 2d
    vec4 pixel_color = gl_Color;
    vec4 coul_computed = vec4(back_color[0], back_color[1], back_color[2], alpha);

    float dist2 = (pix_coord.x - pos_mouse.x) * (pix_coord.x - pos_mouse.x) + (pix_coord.y - pos_mouse.y)*(pix_coord.y - pos_mouse.y);
    float dist = sqrt(dist2);
    if(dist >= radius) {
        // on ecrase les parties non utiles
        float f = dist;
        pixel_color = (20 / f) * coul_computed ;
    }

    vec2 pos =  gl_TexCoord[0].xy; // 4d coord texture    
    vec4 textureVers4D = texture2D(texture, pos);
    gl_FragColor = textureVers4D * pixel_color;
}