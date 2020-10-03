#ifndef PBRDIFFUSE_H
#define PBRDIFFUSE_H
#include<direct.h>
#include"RenderLoop.h"
class PBR_Diffuse:public RenderLoop
{
public:
	PBR_Diffuse() {
		initDemo();
	};

	Shader *commonShader;
	RenderedSphere *sphereObj;

	glm::vec3 lightPos;

	float LPos[3];
	float alpha = 0.35;
	bool showSpec = false;
	int type=1;
	void initDemo()
	{
		SetDemoName("PBR_Diffuse");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
		for (int i = 0; i < 3; i++)
			LPos[i] = lightPos[i];
	}

	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/PBR/pbr_g.vert", "../../Core/PBR/pbr_diffuse.frag");

		sphereObj = new RenderedSphere();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		MouseMove = false;
	}

	void update()
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1, 0.1, 0.1, 1.0);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 view = camera.GetViewMatrix();

		commonShader->use();

		commonShader->setMat4("view", view);
		commonShader->setMat4("projection", projection);
		commonShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		commonShader->setVec3("lightPos", lightPos);
		commonShader->setVec3("viewPos", camera.Position);

		commonShader->setFloat("alpha", alpha);
		commonShader->setInt("type", type);
		commonShader->setBool("showSpec", showSpec);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
		commonShader->setMat4("model", model);
		sphereObj->drawObject();

	}
	void exit()
	{
		commonShader->exit();
		sphereObj->exit();

		delete commonShader;
		delete sphereObj;

		MouseMove = true;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("PBR_Diffuse", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("ShowSpec", &showSpec);
		ImGui::SliderFloat("alpha", &alpha, 0, 1);
		ImGui::SliderInt("type", &type,1,10);	
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		for (int i = 0; i < 3; i++)
			lightPos[i] = LPos[i];

		ImGui::End();
	}
};
#endif