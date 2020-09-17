#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

out vec2 TexCoords;
void main()
{
    gl_Position=vec4(-1,-1,0,1.0);
    TexCoords=vec2(0,0);
    EmitVertex();
    gl_Position=vec4(1,1,0,1.0);
    TexCoords=vec2(1,1);
    EmitVertex();
    gl_Position=vec4(-1,1,0,1.0);
    TexCoords=vec2(0,1);
    EmitVertex();
    EndPrimitive();

    gl_Position=vec4(-1,-1,0,1.0);
    TexCoords=vec2(0,0);
    EmitVertex();
    gl_Position=vec4(1,-1,0,1.0);
    TexCoords=vec2(1,0);
    EmitVertex();
    gl_Position=vec4(1,1,0,1.0);
    TexCoords=vec2(1,1);
    EmitVertex();
    EndPrimitive();
}