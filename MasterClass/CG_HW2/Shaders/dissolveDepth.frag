#version 330
uniform sampler2D NoiseTexture;
uniform float DissolveT;

in vec2 UV;

void main() {
    float noiseValue = texture(NoiseTexture, UV).r;
	if (noiseValue < DissolveT) {
		discard;
	}
}