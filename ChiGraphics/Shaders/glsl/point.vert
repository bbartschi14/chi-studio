#version 330 core

uniform float isSelected;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 camera_position;

layout(location = 0) in vec3 vertex_position;

void main() {
    vec3 world_position = vec3(model_matrix * 
        vec4(vertex_position, 1.0));

    // Move points towards camera to separate from mesh
    world_position += (normalize(camera_position - world_position) * .1);

    //isSelected = gl_VertexID == 10 ?  1.0 : 0.0;
    gl_PointSize = isSelected > 0.5 ? 8.0 : 4.0;
    gl_Position = projection_matrix * view_matrix * vec4(world_position, 1.0);
}