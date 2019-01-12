#version 430

layout(points) in;
layout(points, max_vertices=1) out;

uniform mat4 Projection;
uniform mat4 ModelView;


void main()
{       
   gl_Position = gl_in[0].gl_Position;  
   EmitVertex();
   EndPrimitive();
}