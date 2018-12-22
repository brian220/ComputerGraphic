#version 430
precision mediump float;
layout(binding = 0) uniform sampler2D Texture;

uniform vec4 Light;
in vec2 frag_UV;
in vec3 frag_position;
in vec3 frag_normal;
out vec4 frag_color;

void main() {
    vec3 l = normalize(vec3(Light) - frag_position);
    vec3 n = normalize(vec3(frag_normal));
    vec3 v = vec3(-frag_position);
    vec3 r = reflect(-l, n);
    vec3 ambient = 0.1 * vec3(0.7, 0.7, 0.7);
    vec3 diffuse = 1.0 * vec3(texture2D(Texture, frag_UV)) * max(dot(n,l), 0);
    vec3 specular = vec3(0.0,0.0,0.0);
    if(max(dot(n,l), 0) > 0) {
      specular = 0.1 * vec3(0.8, 0.8, 0.8) * pow(max(dot(v,r), 0), 4.0) * vec3(texture2D(Texture, frag_UV));
    }
    frag_color = vec4(ambient + diffuse + specular , 1.0);
}
