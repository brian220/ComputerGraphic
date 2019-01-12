#version 430
precision mediump float;
layout(binding = 0) uniform sampler2D Texture;
layout(binding = 1) uniform sampler2D NormalMap;
layout(binding = 2) uniform sampler2D SpecularMap;

uniform vec4 Light;

in vec2 frag_UV;
in vec3 frag_position;
in vec3 frag_normal;

in vec3 tan;
in vec3 btan;
out vec4 frag_color;

void main() {
    vec3 color = vec3(texture2D(Texture, frag_UV));
    vec3 normal = mat3(tan, btan, frag_normal) * normalize(2*(texture2D(NormalMap, frag_UV).rgb) - vec3(1.0));
    float specular_factor = texture2D(SpecularMap, frag_UV).z;

    vec3 l = normalize(vec3(Light) - frag_position);
    vec3 n = normalize(vec3(normal));
    vec3 v = vec3(-frag_position);
    vec3 r = reflect(-l, n);
    vec3 ambient = 0.5 * vec3(0.7, 0.7, 0.7) * color;
    vec3 diffuse = 1.0 * color * max(dot(n,l), 0);
    vec3 specular = vec3(0.0,0.0,0.0);
    if(max(dot(n,l), 0) > 0) {
      specular = 0.1 * vec3(0.8, 0.8, 0.8) * pow(max(dot(v,r), 0), 4.0) * color * specular_factor;
    }
    //frag_color = vec4(ambient + diffuse + specular , 1.0);
	frag_color = vec4(1.0, 1.0, 1.0, 0.0);
}
