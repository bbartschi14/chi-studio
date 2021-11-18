#version 330 core

in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D in_texture1;
uniform sampler2D in_texture2;

void main() 
{
    // Alpha Blend
    float newAlpha = vec4(texture(in_texture2, tex_coord)).a;
    vec4 newColor = vec4(texture(in_texture2, tex_coord).rgb, 1.0);
    vec4 previousColor = vec4(texture(in_texture1, tex_coord).rgb, 1.0);

    frag_color = newAlpha * newColor + (1 - newAlpha) * previousColor;
}
