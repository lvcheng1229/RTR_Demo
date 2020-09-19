
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
	Shader *NoTexShader;

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

	void SettingBeforLoop()
	{			
		NoTexShader = new Shader("../../Core/Lines/noTexShader.vert", "../../Core/Lines/noTexShader.frag","../../Core/Lines/noTexShader.geom");

		bunny = new Model("../../Core/PublicResource/bunny.obj");
	
		glGenTextures(1, &LineTex);
		glBindTexture(GL_TEXTURE_1D, LineTex);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load("../../Core/Lines/edgeTex.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			GLint format;
			if (nrChannels == 1)
				format = GL_RED;
			if(nrChannels==3)
				format = GL_RGB;
			if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, format,GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			stbi_image_free(data);
		}
		else
		{
			cout << "failed to loat texture:../../Core/Lines/edgeTex.png" << endl;
			stbi_image_free(data);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();

		NoTexShader->use();
		NoTexShader->setInt("lineTex", 0);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		NoTexShader->use();
		NoTexShader->setMat4("projection", projection);
		NoTexShader->setMat4("view", view);
		NoTexShader->setVec3("viewPos", camera.Position);
		NoTexShader->setVec3("lightPos", lightPos);
		NoTexShader->setVec3("lightcolor", glm::vec3(1, 1, 1));
		NoTexShader->setVec3("objColor", glm::vec3(0.8, 0.8, 0.2));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, LineTex);

		renderScene(NoTexShader);
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

		delete renderCube;
		delete renderSphere;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("NPRLines", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);

		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif