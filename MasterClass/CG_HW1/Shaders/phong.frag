#version 330
uniform sampler2D Texture;
uniform vec4 LightPosition;

in vec2 UV;
in vec3 MV_position;
in vec3 frag_normal;

out vec4 frag_color;

void main() {
    vec3 color = vec3(texture(Texture, UV));
	
	// Phong shading
	float Ka = 1.0;
	float Kd = 1.0;
	float Ks = 1.0;
	float alpha = 3.6;
	vec3 La = vec3(0.2, 0.2, 0.2);
	vec3 Ld = vec3(0.5, 0.5, 0.5);
	vec3 Ls = vec3(0.8, 0.8, 0.8);
	
	vec3 l = normalize(vec3(LightPosition) - MV_position);
	vec3 n = frag_normal;
	vec3 v = normalize(vec3(-MV_position));
	vec3 r = normalize(reflect(-l, n));
	
	// Ambient
    vec3 ambient = Ka * La * color;
    	
	// Diffuse
    vec3 diffuse = Kd * max(dot(n,l), 0) * Ld * color;
	
	//Specular
	vec3 specular = vec3(0.0,0.0,0.0);
	if (dot(n,l) > 0) {
	    specular = Ks * Ls * pow(max(dot(v,r), 0), alpha) * color;
	}
	
	frag_color = vec4(ambient + diffuse + specular, 1.0);
}
