#version 330 core
out vec4 Fragcolor;
uniform vec3 edgeColor;
void main()
{
    Fragcolor=vec4(edgeColor,1.0);
}