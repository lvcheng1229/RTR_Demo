
#ifndef NPRTEST_H
#define NPRTEST_H
#include<direct.h>
#include"RenderLoop.h"
class NPRTEST:public RenderLoop
{
public:
	NPRTEST() {
		initDemo();
	};
	Shader *NoTexShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *bunny;

	GLuint containerTexture;
	glm::vec3 lightPos;

	float LPos[3];
	void initDemo()
	{
		SetDemoName("NPRTEST");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);


		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];
		}
	}

	void SettingBeforLoop()
	{			
		containerTexture = loadTexture("../../Core/PublicResource/container.jpg");
		NoTexShader = new Shader("../../Core/PublicResource/noTexShader.vert", "../../Core/PublicResource/noTexShader.frag");
		
		bunny = new Model("../../Core/PublicResource/bunny.obj");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();
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
		glDeleteTextures(1, &containerTexture);

		renderCube->exit();
		renderSphere->exit();

		delete renderCube;
		delete renderSphere;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("NPRTEST", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);

		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif