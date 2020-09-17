#version 330 core
out vec4 Fragcolor;
in vec2 TexCoords;

uniform sampler2D gColor;
uniform sampler2D gNormal;
uniform sampler2D depthTex;

uniform vec3 edgeColor;

float sobelDepthGx(sampler2D tex,vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    float result=0.0;
    result+=-1.0*texture(tex,texCoords+vec2(-1,1)*texelSize).x;
    result+=-2.0*texture(tex,texCoords+vec2(0,1)*texelSize).x;
    result+=-1.0*texture(tex,texCoords+vec2(1,1)*texelSize).x;

    result+=1.0*texture(tex,texCoords+vec2(-1,-1)*texelSize).x;
    result+=2.0*texture(tex,texCoords+vec2(0,-1)*texelSize).x;
    result+=1.0*texture(tex,texCoords+vec2(1,-1)*texelSize).x;

    return result;
}
float sobelDepthGy(sampler2D tex,vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    float result=0.0;
    result+=-1.0*texture(tex,texCoords+vec2(-1,1)*texelSize).x;
    result+=-2.0*texture(tex,texCoords+vec2(-1,0)*texelSize).x;
    result+=-1.0*texture(tex,texCoords+vec2(-1,-1)*texelSize).x;

    result+=1.0*texture(tex,texCoords+vec2(1,1)*texelSize).x;
    result+=2.0*texture(tex,texCoords+vec2(1,0)*texelSize).x;
    result+=1.0*texture(tex,texCoords+vec2(1,-1)*texelSize).x;

    return result;
}
float comEdgeByDepth(sampler2D tex,vec2 texCoords)
{
    float res1=sobelDepthGx(tex,texCoords);
    float res2=sobelDepthGy(tex,texCoords);
    return (abs(res1)+abs(res2))/2.0;
}




vec3 sobelVec3Gy(sampler2D tex,vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    vec3 result=vec3(0.0);
    result+=-1.0*texture(tex,texCoords+vec2(-1,1)*texelSize).xyz;
    result+=-2.0*texture(tex,texCoords+vec2(-1,0)*texelSize).xyz;
    result+=-1.0*texture(tex,texCoords+vec2(-1,-1)*texelSize).xyz;

    result+=1.0*texture(tex,texCoords+vec2(1,1)*texelSize).xyz;
    result+=2.0*texture(tex,texCoords+vec2(1,0)*texelSize).xyz;
    result+=1.0*texture(tex,texCoords+vec2(1,-1)*texelSize).xyz;

    return result;
}
vec3 sobelVec3Gx(sampler2D tex,vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    vec3 result=vec3(0.0);
    result+=-1.0*texture(tex,texCoords+vec2(-1,1)*texelSize).xyz;
    result+=-2.0*texture(tex,texCoords+vec2(0,1)*texelSize).xyz;
    result+=-1.0*texture(tex,texCoords+vec2(1,1)*texelSize).xyz;

    result+=1.0*texture(tex,texCoords+vec2(-1,-1)*texelSize).xyz;
    result+=2.0*texture(tex,texCoords+vec2(0,-1)*texelSize).xyz;
    result+=1.0*texture(tex,texCoords+vec2(1,-1)*texelSize).xyz;

    return result;
}
float comEdge(sampler2D tex,vec2 texCoords)
{
    vec3 res1=sobelVec3Gx(tex,texCoords);
    vec3 res2=sobelVec3Gy(tex,texCoords);
    float a=abs(res1.x+res1.y+res1.z)/3.0;
    float b=abs(res2.x+res2.y+res2.z)/3.0;
    return (a+b)/2.0;
}


void main()
{

    float edge=0.0;
    float tempValue;

    tempValue=comEdgeByDepth(depthTex,TexCoords);
    if(tempValue>0.5)edge=1.0;

    tempValue=comEdge(gNormal,TexCoords);
    if(tempValue>0.5)edge=1.0;

    vec3  color=texture(gColor,TexCoords).xyz;
    if(edge>0.5)color=edgeColor;
    
    Fragcolor=vec4(color,1.0);
}