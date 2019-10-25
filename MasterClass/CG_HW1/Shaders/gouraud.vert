#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform vec4 LightPosition;

out vec2 UV;
out vec3 frag_intensity;

uniform mat4 Projection;
uniform mat4 ModelView;

void main() {
    gl_Position = Projection * ModelView * vec4(position, 1.0);
  
    mat3 normal_matrix = transpose(inverse(mat3(ModelView)));
    vec3 normal = normalize(normal_matrix * normal);
  
    vec3 MV_position =  vec3(ModelView * vec4(position, 1.0));
  
    UV = texcoord;
    
    float Ka = 1.0;
	float Kd = 1.0;
	float Ks = 1.0;
	float alpha = 3.6;
	vec3 La = vec3(0.2, 0.2, 0.2);
	vec3 Ld = vec3(0.5, 0.5, 0.5);
	vec3 Ls = vec3(0.8, 0.8, 0.8);
	
	vec3 l = normalize(vec3(LightPosition) - MV_position);
	vec3 n = normal;
	vec3 v = normalize(vec3(-MV_position));
	vec3 r = normalize(reflect(-l, n));
	
	// Ambient
    vec3 ambient = Ka * La;
    	
	// Diffuse
    vec3 diffuse = Kd * max(dot(n,l), 0) * Ld;
	
	//Specular
	vec3 specular = vec3(0.0,0.0,0.0);
	if (dot(n,l) > 0) {
	    specular = Ks * Ls * pow(max(dot(v,r), 0), alpha);
	}
	
	frag_intensity = ambient + diffuse + specular;
}
