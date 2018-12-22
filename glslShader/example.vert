#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec2 geom_UV;
out vec3 geom_position;
//out vec3 geom_light_position;
out vec3 geom_normal;

uniform mat4 Projection;
uniform mat4 ModelView;

void main() {
  geom_UV = texcoord;
  gl_Position = Projection * ModelView * vec4(position, 1.0);
  vec4 vertPos4 = ModelView * vec4(position, 1.0);
  geom_position = vec3(vertPos4) / vertPos4.w;
  //geom_normal = normalize(vec3(Projection * ModelView * vec4(normal, 1.0)));
  mat3 normal_matrix = transpose(inverse(mat3(ModelView)));
  geom_normal = normalize(normal_matrix * normal);
}
