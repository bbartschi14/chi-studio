#version 330 core

uniform float isSelected;

out vec4 frag_color;

void main() { 
    frag_color = isSelected > 0.5 ? vec4(.97, 0.55, 0.05, 1.0) :  vec4(vec3(.05), 1.0);
}

