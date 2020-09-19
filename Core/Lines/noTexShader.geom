#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 normal;
	vec3 FragPos;
} gs_in[];

out GS_OUT {
    vec3 normal;
	vec3 FragPos;
    vec3 lineTexCoords;
} gs_out;
void main()
{
    gl_Position=gl_in[0].gl_Position;
    gs_out.normal=gs_in[0].normal;
    gs_out.FragPos=gs_in[0].FragPos;
    gs_out.lineTexCoords=vec3(0.0,1.0,1.0);
    EmitVertex();

    gl_Position=gl_in[1].gl_Position;
    gs_out.normal=gs_in[1].normal;
    gs_out.FragPos=gs_in[1].FragPos;
    gs_out.lineTexCoords=vec3(1.0,0.0,1.0);
    EmitVertex();

    gl_Position=gl_in[2].gl_Position;
    gs_out.normal=gs_in[2].normal;
    gs_out.FragPos=gs_in[2].FragPos;
    gs_out.lineTexCoords=vec3(1.0,1.0,0.0);
    EmitVertex();

    EndPrimitive();
}