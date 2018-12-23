#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec2 geom_UV[3];
in vec3 geom_position[3];
in vec3 geom_light_position[3];
in vec3 geom_normal[3];

out vec2 frag_UV;
out vec3 frag_position;
out vec3 frag_light_position;
out vec3 frag_normal;

out vec3 tan;
out vec3 btan;
void compute_frag_normal_map();
void main()
{
  compute_frag_normal_map();
  for(int i = 0; i < gl_in.length(); i++)
  {
    gl_Position = gl_in[i].gl_Position;
    frag_UV = geom_UV[i];
    frag_position = geom_position[i];
    frag_normal = geom_normal[i];
    EmitVertex();
  }
  EndPrimitive();
}

void compute_frag_normal_map() {
  vec3 edg1 = geom_position[1] - geom_position[0];
  vec3 edg2 = geom_position[2] - geom_position[0];
  vec2 deltaUV1 = geom_UV[1] - geom_UV[0];
  vec2 deltaUV2 = geom_UV[2] - geom_UV[0];
  mat2 uv = transpose(mat2(deltaUV1, deltaUV2));
  mat3x2 edg = transpose(mat2x3(edg1, edg2));
  mat3x2 tb = inverse(uv) * edg;
  tan = transpose(tb)[0];
  btan = transpose(tb)[1];
}
