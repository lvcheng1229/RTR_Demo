#version 330 core
out  vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D gColor;
uniform sampler2D ssrColor;
void main()
{
    vec2 texSize=textureSize(ssrColor, 0).xy;
    vec3 uvv=texture(ssrColor,TexCoords).xyz;
    
    vec3 oringinColor=texture(gColor,TexCoords).xyz;
    if(uvv.x<=0||uvv.y<=0||uvv.x>1||uvv.y>1)
        FragColor=vec4(vec3(1.0),1.0);
    else
    {
        vec3 res=vec3(0);
        for(float i=-2;i<=2;++i)
        {
            for(float j=-2;j<=2;++j)
            {
                res+=texture(gColor,uvv.xy+vec2(i,j)/texSize).xyz;
            }
        }
        res/=25.0;
        //res=texture(gColor,uvv.xy).xyz;
        FragColor=vec4(res*0.5+oringinColor*0.5,1.0);
    }
        
//FragColor=vec4(texture(ssrColor,TexCoords).zw,0.0,1.0);    
}