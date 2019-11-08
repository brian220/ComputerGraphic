#version 330
precision mediump float;
uniform sampler2D Texture;
uniform sampler2D DepthTexture;

in vec2 UV;
in vec3 FragPos;
in vec3 FragLightPos;
in vec3 FragNormal;
in vec4 FragPosLightSpace;

out vec4 frag_color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0, 1] -> range of depth
	projCoords = projCoords * 0.5 + 0.5;
	// Get closest depth from the depth buffer
	float closestDepth = texture(DepthTexture, projCoords.xy).r;
	// Get the current depth
	float currentDepth = projCoords.z;
	
	// Bias for shadow acne
	vec3 lightDir = normalize(FragLightPos - FragPos);
	float bias = max(0.05 * (1.0 - dot(FragNormal, lightDir)), 0.005);
	
	// Comppare current depth with cloest depth
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	
	if (projCoords.x >1.0 || projCoords.y > 1.0 || projCoords.z > 1.0)
		shadow = 0.0;
	if (projCoords.x < 0.0 || projCoords.y < 0.0 || projCoords.z < 0.0)
	    shadow = 0.0;
	
	return shadow;
}

void main() {
	float shadow = ShadowCalculation(FragPosLightSpace);
	vec4 texColor = texture(Texture, UV);
	frag_color = texColor * (1.0 - shadow);
}

