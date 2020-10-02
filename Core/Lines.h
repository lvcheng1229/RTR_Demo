
#ifndef NPRLINES_H
#define NPRLINES_H
#include<direct.h>
#include"RenderLoop.h"
class NPRLines:public RenderLoop
{
public:
	NPRLines() {
		initDemo();
	};
	Shader *LineShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;
	Model *bunny;

	glm::vec3 lightPos;
	GLuint LineTex;

	float LPos[3];
	void initDemo()
	{
		SetDemoName("NPRLines");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);


		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];
		}
	}

	void LoadTexForLevel(int level, const char *path)
	{
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			GLint format;
			if (nrChannels == 1)
				format = GL_RED;
			if(nrChannels==3)
				format = GL_RGB;
			if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage1D(GL_TEXTURE_1D, level, format, width, 0, format,GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

			stbi_image_free(data);
		}
		else
		{
			cout << "failed to loat texture:../../Core/Lines/edgeTex.png" << endl;
			stbi_image_free(data);
		}
	}

	void SettingBeforLoop()
	{			
		//Paper:Fast and versatile texture-based wireframe rendering
		
		LineShader = new Shader("../../Core/Lines/noTexShader.vert", "../../Core/Lines/noTexShader.frag","../../Core/Lines/noTexShader.geom");
		bunny = new Model("../../Core/PublicResource/bunny.obj");
	
		glGenTextures(1, &LineTex);
		glBindTexture(GL_TEXTURE_1D, LineTex);

		//一个常见的错误是，将放大过滤的选项设置为多级渐远纹理过滤选项之一。
		//这样没有任何效果，因为多级渐远纹理主要是使用在纹理被缩小的情况下的：
		//纹理放大不会使用多级渐远纹理，为放大过滤设置多级渐远纹理的选项会产生一个

		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 2);

		LoadTexForLevel(0, "../../Core/Lines/edgeLine0.png");
		LoadTexForLevel(1, "../../Core/Lines/edgeLine1.png");
		LoadTexForLevel(2, "../../Core/Lines/edgeLine2.png");
	
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();

		LineShader->use();
		LineShader->setInt("lineTex", 0);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		LineShader->use();
		LineShader->setMat4("projection", projection);
		LineShader->setMat4("view", view);
		LineShader->setVec3("viewPos", camera.Position);
		LineShader->setVec3("lightPos", lightPos);
		LineShader->setVec3("lightcolor", glm::vec3(1, 1, 1));
		LineShader->setVec3("objColor", glm::vec3(0.8, 0.8, 0.2));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, LineTex);

		renderScene(LineShader);
	}
	void renderScene(Shader *shader)
	{		
		glm::mat4 model = glm::mat4(1.0f);

		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();

		//sphere
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderSphere->drawObject();

		//bunny
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -1.3f, -3.0f));
		model = glm::scale(model, glm::vec3(8.0f));
		shader->setMat4("model", model);
		bunny->Draw(*shader);
	}
	void exit()
	{
		glDeleteTextures(1, &LineTex);

		renderCube->exit();
		renderSphere->exit();
		bunny->exit();

		LineShader->exit();

		delete renderCube;
		delete renderSphere;
		delete bunny;

		delete LineShader;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("NPRLines", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);

		for (int i = 0; i < 3; i++)lightPos[i] = LPos[i];

		ImGui::End();
	}
};
#endif