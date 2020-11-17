#ifndef TEXTUREBUFFER_H
#define TEXTUREBUFFER_H
#include<direct.h>
#include"RenderLoop.h"
struct Light
{
	glm::vec3 positions;
	glm::vec3 color;
	glm::vec3 direction;
	float radius;
};
class TextureBuffer
{
public:
	unsigned int _glTexture;
	int _pixelsPerElement;
	int _elementCount;

	float *_buffer;
	TextureBuffer(int elementCount, int elementSize)
	{
		glGenTextures(1, &_glTexture);
		glBindTexture(GL_TEXTURE_2D, _glTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_pixelsPerElement = ceil(elementSize / 4.0);
		_elementCount = elementCount;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, elementCount, _pixelsPerElement, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		_buffer = new float[_elementCount * 4 * _pixelsPerElement];
	}
	void exit()
	{
		glDeleteTextures(1,& _glTexture);

		delete[]_buffer;
	}
	int bufferIndex(int index, int component)
	{
		return 4 * index + 4 * component*_elementCount;
	}
	void update()
	{
		glBindTexture(GL_TEXTURE_2D, _glTexture);
		//glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, _elementCount, _pixelsPerElement,  GL_RGBA, GL_FLOAT, _buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _elementCount, _pixelsPerElement, 0, GL_RGBA, GL_FLOAT, _buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
#endif