#ifndef MODELDEMO_H
#define MODELDEMO_H
#include<direct.h>
#include"../Core/RenderLoop.h"
#include"../Core/Model.h"
class ModelDemo:public RenderLoop
{
public:
	ModelDemo() {
		initDemo();
	};
	
	Shader *commonShader;	
	Model *mymodel;

	glm::vec3 lightPos;
	void SettingBeforLoop()
	{
		commonShader =new Shader("../../Core/PublicResource/commonshader.vs", "../../Core/PublicResource/commonshader.fs");

		mymodel =new  Model("../../Core/PublicResource/nanosuit/nanosuit.obj");
		commonShader->use();
		commonShader->setInt("sampleTexture", 0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void initDemo()
	{
		SetDemoName("ModelDemo");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
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

		model = glm::translate(model, glm::vec3(0.0f, -5.0f, -10.0f));

		commonShader->setMat4("model", model);

		mymodel->Draw(*commonShader);
	}
	void exit()
	{
		commonShader->exit();
		mymodel->exit();

		delete commonShader;
		delete mymodel;
	}
	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("ModelDemo", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Text("This is ModelDemo");
		ImGui::End();
	}
};

#endif