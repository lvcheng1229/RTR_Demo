#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D tex;
void main()
{
    vec3 color=texture(tex,TexCoords).xyz;
    FragColor=vec4(abs(color),1.0);
}