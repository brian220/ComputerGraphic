layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 DepthMV;
uniform mat4 DepthP;

out vec2 UV;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main() {
    gl_Position = Projection * ModelView * vec4(position, 1.0);
	UV = texcoord;
	FragPosLightSpace = DepthP * DepthMV * vec4(position, 1.0);
}