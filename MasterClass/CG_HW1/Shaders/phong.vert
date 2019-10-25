#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec2 UV;
//flat out vec3 flat_normal;
out vec3 frag_normal;
out vec3 MV_position;

uniform mat4 Projection;
uniform mat4 ModelView;


void main() {
  gl_Position = Projection * ModelView * vec4(position, 1.0);
  
  mat3 normal_matrix = transpose(inverse(mat3(ModelView)));
  frag_normal = normalize(normal_matrix * normal);
  
  MV_position =  vec3(ModelView * vec4(position, 1.0));
  
  UV = texcoord;
}
