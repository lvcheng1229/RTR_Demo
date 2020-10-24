#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 WorldPos;

void main()
{
    //WorldPos = aPos;
	WorldPos=vec3(model*vec4(aPos,1.0));
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(aPos, 1.0);

	gl_Position = clipPos.xyww;
}