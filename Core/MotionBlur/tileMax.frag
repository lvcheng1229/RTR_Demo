#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D velocityAndDepth;
uniform int tile;
void main()
{
    vec2 tex_offset = 1.0 / textureSize(velocityAndDepth, 0);
    vec3 color=vec3(0.0);
    for(int i=-(tile-1)/2;i<=(tile-1)/2;i++)
    {
        for(int j=-(tile-1)/2;j<=(tile-1)/2;j++)
        {
            vec3 tempCol=texture(velocityAndDepth,TexCoords+vec2(i,j)*tex_offset).xyz;
            if(length(color)<length(tempCol))
            {
                color=tempCol;
            }
        }
    }

    FragColor=vec4(color,1);
}