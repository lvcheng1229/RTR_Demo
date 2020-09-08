#ifndef RENDEREDOBJECT_H
#define RENDEREDOBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class RenderedObject
{
public:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO = 0;
	bool haveSetAttribute = false;
	virtual void attributeSetting() {};
	virtual void changeBufferData() {};
	virtual void drawObject() {};
	virtual void exit() {};
};
#endif
