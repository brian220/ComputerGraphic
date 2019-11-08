layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 DepthMV;
uniform mat4 DepthP;
uniform vec3 LightPos;

out vec2 UV;
out vec3 FragPos;
out vec3 FragLightPos;
out vec3 FragNormal;
out vec4 FragPosLightSpace;


void main() {
    gl_Position = Projection * ModelView * vec4(position, 1.0);
	UV = texcoord;
	FragPosLightSpace = DepthP * DepthMV * vec4(position, 1.0);
	
	mat3 normalMatrix = transpose(inverse(mat3(DepthMV)));
	// Transfer normal to view space
	FragNormal = normalize(normalMatrix * normal);
	// Transfer light position to view space
	FragLightPos = vec3(DepthMV * vec4(LightPos, 1.0));
    // Transfer fragment position to view space	
	FragPos = vec3(DepthMV * vec4(position, 1.0));
}