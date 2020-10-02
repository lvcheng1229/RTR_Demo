#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 normal;
out vec3 FragPos;

out vec3 tangent;
out vec3 bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void  main()
{
	//only sphere model can use this method
	vec3 up=vec3(0,1,0);
	tangent=cross(up,aNormal);
	if(abs(dot(up,normalize(aNormal)))>0.99)
	{
		tangent=vec3(1,0,0);
	}
	bitangent=cross(aNormal,tangent);
	//model matrix don't involve scale and rotate in this demo
	//normal=mat3(transpose(inverse(model)))*aNormal;
	normal=aNormal;
	FragPos=vec3(model*vec4(aPos,1.0));
	texCoords=aTexCoords;
	gl_Position=projection*view*model*vec4(aPos,1.0);
}

