#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool zbaisOrShell;
uniform float edgeWidth;
void  main()
{
	vec4 viewPos=view*model*vec4(aPos,1.0);
    vec3 normal=mat3(transpose(inverse(model)))*aNormal;
    if(zbaisOrShell)//zBias
    {
        viewPos.z+=edgeWidth;
    }
    else//shell
    {        
        normal.z-=0.4;
        normal=edgeWidth*normalize(normal);
        viewPos=viewPos+vec4(normal,0);      
    }
    gl_Position=projection*viewPos;
	
}