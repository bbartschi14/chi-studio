#version 330 core

uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 camera_position;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

void main() {
    vec3 world_normal = normal_matrix * vertex_normal;

    vec3 world_position = vec3(model_matrix * 
        vec4(vertex_position, 1.0));

    world_position += world_normal * distance(world_position, camera_position) * .025;

    gl_Position = projection_matrix * view_matrix * vec4(world_position, 1.0);
}