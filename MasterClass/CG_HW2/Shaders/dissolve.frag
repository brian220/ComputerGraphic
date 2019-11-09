#version 330
precision mediump float;
uniform sampler2D Texture;
uniform sampler2D DepthTexture;
uniform sampler2D NoiseTexture;
uniform vec3 LightPos;
uniform float DissolveT;

in vec2 UV;
in vec3 FragPos;
in vec3 FragNormal;
in vec4 FragPosLightSpace;

out vec4 frag_color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0, 1] -> range of depth
	projCoords = projCoords * 0.5 + 0.5;
	// Get the current depth
	float currentDepth = projCoords.z;
	
	// Bias for shadow acne
	vec3 lightDir = normalize(LightPos - FragPos);
	float bias = max(0.01 * (1.0 - dot(FragNormal, lightDir)), 0.001);
	
	// For PCF
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(DepthTexture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(DepthTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
	
	if (projCoords.x > 1.0 || projCoords.y > 1.0 || projCoords.z > 1.0)
		shadow = 0.0;
	if (projCoords.x < 0.0 || projCoords.y < 0.0 || projCoords.z < 0.0)
	    shadow = 0.0;
	
	return shadow;
}

void main() {
	float shadow = ShadowCalculation(FragPosLightSpace);
	vec4 texColor = texture(Texture, UV);
	// For dissolving effect
	float noiseValue = texture(NoiseTexture, UV).r;
	if (noiseValue < DissolveT) {
		 discard;
		// texColor = vec4(vec3(texture(Texture, UV)), 0.0);
	}
	
	frag_color = texColor * (1.0 - shadow);
}

