#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 normal;
out vec3 FragPos;

out vec3 curViewPos;
out vec3 preViewPos;

uniform mat4 preModelView;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void  main()
{
    curViewPos=vec3(view*model*vec4(aPos,1.0));
    preViewPos=vec3(preModelView*vec4(aPos,1.0));

	normal=mat3(transpose(inverse(model)))*aNormal;
	FragPos=vec3(model*vec4(aPos,1.0));
	texCoords=aTexCoords;
	gl_Position=projection*view*model*vec4(aPos,1.0);
}

