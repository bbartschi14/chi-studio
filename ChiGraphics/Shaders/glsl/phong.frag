#version 330 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 mask_color;

struct AmbientLight {
    bool enabled;
    vec3 ambient;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 world_position;
in vec3 world_normal;
in vec2 tex_coord;

uniform vec3 camera_position;

uniform Material material; // material properties of the object
uniform AmbientLight ambient_light;
uniform PointLight point_light; 
vec3 CalcAmbientLight();
vec3 CalcPointLight(vec3 normal, vec3 view_dir);

void main() {
    vec3 normal = normalize(world_normal);
    vec3 view_dir = normalize(camera_position - world_position);

    frag_color = vec4(0.0);

    //if (ambient_light.enabled) {
    if (true) {
        frag_color += vec4(CalcAmbientLight(), 1.0);
    }
    
    if (point_light.enabled) {
        frag_color += vec4(CalcPointLight(normal, view_dir), 1.0);
    }

    mask_color = vec4(1.0, 1.0, 1.0, 1.0);
}

vec3 GetAmbientColor() {
    return material.ambient;
}

vec3 GetDiffuseColor() {
    return material.diffuse;
}

vec3 GetSpecularColor() {
    return material.specular;
}

vec3 CalcAmbientLight() {
    return vec3(.35) * GetAmbientColor();
}

vec3 CalcPointLight(vec3 normal, vec3 view_dir) {
    PointLight light = point_light;
    vec3 light_dir = normalize(camera_position - world_position);

    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
        max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity * 
        light.specular * GetSpecularColor();

    float distance = length(camera_position - world_position);
    //float attenuation = 1.0 / (light.attenuation.x + 
    //    light.attenuation.y * distance + 
    //   light.attenuation.z * (distance * distance));

    float attenuation = .75;

    return attenuation * (diffuse_color + specular_color);
}

