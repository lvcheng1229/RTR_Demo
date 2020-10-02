#ifndef PBRNDF_H
#define PBRNDF_H
#include<direct.h>
#include"RenderLoop.h"
class PBR_NDF:public RenderLoop
{
public:
	PBR_NDF() {
		initDemo();
	};

	Shader *commonShader;
	RenderedSphere *sphereObj;

	glm::vec3 lightPos;
	
	float LPos[3];
	bool showDiffuse = false;
	float alpha = 0.15;
	float alphay = 0.08;
	float gamma = 3;
	int type = 1;
	void initDemo()
	{
		SetDemoName("PBR_NDF");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
		for (int i = 0; i < 3; i++)
			LPos[i] = lightPos[i];
	}

	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/PBR/pnr_ndf.vert", "../../Core/PBR/pbr_ndf.frag");

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

		commonShader->setInt("type", type);
		commonShader->setFloat("alpha", alpha);
		commonShader->setFloat("alphay", alphay);
		commonShader->setFloat("gamma", gamma);

		commonShader->setBool("showDiffuse", showDiffuse);

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
		ImGui::Begin("PBR_NDF", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("ShowDiffuse", &showDiffuse);
		ImGui::SliderFloat("alpha", &alpha, 0, 1);
		ImGui::SliderFloat("alpha(For Anisotropic NDF)", &alphay, 0, 1);
		ImGui::SliderFloat("gamma(For Hyper_Cauchy)", &gamma, 2, 15);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::Text("Type1:Blinn\nType2:Beckmann\ntype3:GGX\ntype4:GGXAnisotropic\ntype5:BeckmannAnisotropicntype\ntype6:Hyper_Cauchy");
		ImGui::SliderInt("Type",&type,1,6);	
		for (int i = 0; i < 3; i++)
			lightPos[i] = LPos[i];

		ImGui::End();
	}
};
#endif