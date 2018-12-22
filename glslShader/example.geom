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

//in vec4 geom_color[3];
//out vec4 frag_color;

void main()
{
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
