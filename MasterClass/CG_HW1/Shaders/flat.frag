#version 330
uniform sampler2D Texture;

in vec2 UV;

flat in vec3 frag_intensity;
out vec4 frag_color;

void main() {
	// Gouraud shading
    vec3 color = vec3(texture(Texture, UV));
	frag_color = vec4(frag_intensity * color, 1.0); 
}
