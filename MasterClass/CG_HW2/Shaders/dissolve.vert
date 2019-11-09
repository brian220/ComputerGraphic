layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 DepthM;
uniform mat4 DepthV;
uniform mat4 DepthP;

out vec2 UV;
out vec3 FragPos;
out vec3 FragNormal;
out vec4 FragPosLightSpace;

void main() {
    gl_Position = Projection * ModelView * vec4(position, 1.0);
	UV = texcoord;
	
	FragPosLightSpace = DepthP * DepthV * DepthM * vec4(position, 1.0);
	
	mat3 normalMatrix = transpose(inverse(mat3(DepthM)));
	// Transfer normal to world space
	FragNormal = normalize(normalMatrix * normal);
    // Transfer fragment position to world space	
	FragPos = vec3(DepthM * vec4(position, 1.0));
}