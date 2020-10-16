#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D testTex;
void main()
{
    vec3 rr=texture(testTex,TexCoords).rgb;

    FragColor=vec4(vec3(rr),1);
}