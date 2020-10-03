#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

out vec3 normal;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void  main()
{
	normal=mat3(transpose(inverse(model)))*aNormal;
	FragPos=vec3(view*model*vec4(aPos,1.0));
	gl_Position=projection*view*model*vec4(aPos,1.0);
}