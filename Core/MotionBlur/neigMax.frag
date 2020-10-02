#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tileMax;
void main()
{
    vec2 tex_offset = 1.0 / textureSize(tileMax, 0);
    vec3 color=vec3(0.0);
    for(int i=-1;i<=1;i++)
    {
        for(int j=-1;j<=1;j++)
        {
            vec3 tempCol=texture(tileMax,TexCoords+vec2(i,j)*tex_offset).xyz;
            if(length(color)<length(tempCol))
            {
                color=tempCol;
            }
        }
    }
    FragColor=vec4(color,1.0);
}