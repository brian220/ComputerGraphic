#version 430

layout(location = 0) in vec3 pos;

uniform mat4 Projection;
uniform mat4 ModelView;


void main() {
	gl_Position = Projection * ModelView * vec4(pos, 1.0);

}