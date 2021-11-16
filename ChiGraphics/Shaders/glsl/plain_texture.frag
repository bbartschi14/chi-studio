#version 330 core

in vec2 tex_coord;
out vec4 frag_color;

uniform float near_plane;
uniform float far_plane;

uniform sampler2D in_texture;
uniform bool is_depth;

float linearize(float depth)
{
    return (-far_plane * near_plane / (depth * (far_plane - near_plane) - far_plane)) / far_plane;
}

void main() {
    if (is_depth) {
        float depth = texture(in_texture, tex_coord.xy).r;
        float grey = (2.0 * near_plane) / (far_plane + near_plane - depth*(far_plane-near_plane));
        frag_color = vec4(vec3(grey), 1.0);
        frag_color = texture(in_texture, tex_coord);

    } else {
        frag_color = vec4(texture(in_texture, tex_coord).rgb, 1.0);
    }
}
