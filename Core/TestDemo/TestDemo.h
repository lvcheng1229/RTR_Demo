#ifndef TESTDEMO_H
#define TESTDEMO_H
#include<direct.h>
#include"../Core/RenderLoop.h"
class TestDemo:public RenderLoop
{
public:
	TestDemo() {
		initDemo();
	};
	
	Shader *commonShader;
	RenderedSphere *sphereObj;

	GLuint woodTexture;
	glm::vec3 lightPos;

	void initDemo()
	{
		SetDemoName("TestDemo");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}

	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/PublicResource/commonshader.vs", "../../Core/PublicResource/commonshader.fs");

		woodTexture = loadTexture("../../Core/PublicResource/container.jpg");
		
		commonShader->use();
		commonShader->setInt("sampleTexture", 0);

		sphereObj = new RenderedSphere();
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	
	}

	void update()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		glm::mat4 view = camera.GetViewMatrix();

		commonShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		commonShader->setMat4("view", view);
		commonShader->setMat4("projection", projection);
		commonShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		commonShader->setVec3("lightPos", lightPos);
		commonShader->setVec3("viewPos", camera.Position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));

		commonShader->setMat4("model", model);
		sphereObj->drawObject();
	}

	void exit()
	{
		glDeleteTextures(1, &woodTexture);
		commonShader->exit();
		sphereObj->exit();

		delete commonShader;
		delete sphereObj;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("TestDemo", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Text("This is TestDemo");
		ImGui::End();
	}
};
#endif