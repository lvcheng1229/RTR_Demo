#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D originalColor;
uniform sampler2D NearFieldColor;
uniform sampler2D restColor;

uniform float focusDistance;
uniform float focusRange;

uniform bool showNearFieldProblem;
void main()
{             
    vec4 originalCol=texture(originalColor,TexCoords).rgba;
    vec3 nearFieldCol=texture(NearFieldColor,TexCoords).rgb;
    vec3 restCol=texture(restColor,TexCoords).rgb;
    
    float D=originalCol.a;


    vec3 resultCol=vec3(0.0);
    if(D<(focusDistance-focusRange))
    {
        resultCol=nearFieldCol;
    }
    else if(D<focusDistance)
    {
        float lerpValue=(focusDistance-D)/focusRange;
        lerpValue=clamp(lerpValue,0.0,1.0);
        resultCol=originalCol.xyz*(1-lerpValue*lerpValue)+nearFieldCol*lerpValue*lerpValue;
        if(showNearFieldProblem)
            resultCol=originalCol.xyz;
    }
    else if(D<(focusDistance+focusRange))
    {
        resultCol=originalCol.xyz;
    }
    else
    {
        resultCol=restCol;
    }

    FragColor=vec4(resultCol,1.0);
}