layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texcoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 UV;

void main() {
    gl_Position = Projection * View * Model * vec4(position, 1.0);
	UV = texcoord;
}