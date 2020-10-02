#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTex;
uniform sampler2D velocityAndDepth;
uniform sampler2D neigMaxTex;

uniform int sampleNum;
uniform float SOFT_Z_EXTENT;
float rand(vec2 co){
 return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float softDepthCompare(float za,float zb)
{
    return clamp(1-(za-zb)/SOFT_Z_EXTENT,0,1);
}
float cone(vec2 x,vec2 y,vec2 v)
{
    return clamp(1-length(x-y)/length(v),0,1);
}
float cylinder(vec2 x,vec2 y,vec2 v)
{
   return 1.0-smoothstep(0.95*length(v),1.05*length(v),length(x-y));
}
void main()
{     
    vec2 tex_offset = 1.0 / textureSize(colorTex, 0);
    //Largest velocity in the neighborhood
    vec3 maxV=texture(neigMaxTex,TexCoords).xyz;
    
    vec3 Cx=texture(colorTex,TexCoords).xyz;
    if(length(maxV)<0.001)
    {
        FragColor = vec4(Cx, 1.0);//No blur
        return;
    }

    vec4 vad=texture(velocityAndDepth,TexCoords);
    vec3 Vx=vad.xyz;
    float Zx=vad.w;

    //sample the current point
    float weight=1.0;
    vec3 sum=Cx*weight;

    float j=0.0;

    vec2 X=TexCoords;
    //take S-1 additional neighbor samples
    for(int i=0;i<sampleNum;i++)
    {
        j=rand(vec2(Cx.xy))*0.5;
        //choose evenly placed filter taps along +-maxV
        float t=mix(-1.0,1.0,(i+j+1.0)/(sampleNum+1.0));

        vec2 Y=TexCoords+maxV.xy*t*tex_offset;
        vec4 vadY=texture(velocityAndDepth,Y);

        float b=softDepthCompare(Zx,vadY.w);
        float f=softDepthCompare(vadY.w,Zx);

        //避免背景动而前景不动使得前景被模糊
        float alpha=f*cone(Y,X,vadY.xy)+
        b*cone(X,Y,vad.xy)+
        cylinder(Y,X,vadY.xy)*cylinder(X,Y,vad.xy)*2;

        sum+=texture(colorTex,Y).xyz*alpha;
        weight+=alpha;
    }
    sum/=weight;
    FragColor = vec4(sum, 1.0);
}