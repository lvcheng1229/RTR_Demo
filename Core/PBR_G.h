#ifndef PBRG_H
#define PBRG_H
#include<direct.h>
#include"RenderLoop.h"
class PBR_G:public RenderLoop
{
public:
	PBR_G() {
		initDemo();
	};

	Shader *commonShader;
	RenderedSphere *sphereObj;

	glm::vec3 lightPos;

	float LPos[3];
	bool showDiffuse = false;
	float alpha = 0.5;
	float alphay = 0.3;
	int type=1;
	void initDemo()
	{
		SetDemoName("PBR_G");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
		for (int i = 0; i < 3; i++)
			LPos[i] = lightPos[i];
	}

	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/PBR/pbr_g.vert", "../../Core/PBR/pbr_g.frag");

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


		commonShader->setBool("showDiffuse", showDiffuse);
		commonShader->setFloat("alpha", alpha);
		commonShader->setFloat("alphay", alphay);
		commonShader->setInt("type", type);

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
		ImGui::Begin("PBR_G", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("ShowDiffuse", &showDiffuse);
		ImGui::SliderFloat("alpha", &alpha, 0, 1);
		ImGui::SliderFloat("alpha(For Anisotropic G)", &alphay, 0, 1);
		ImGui::Text("notice that in this demo,Vis=frac(G,(4*NoL*Nov)),so G=vis*(4*NoL*Nov)\ntype1:Implicit\ntype2:Neumann[1999]\ntype3:Kelemen[2001]\ntype4:Schlick GGX[1994][important]\ntype5:Smith used in Disney[2007]\n\ntype 6 7 8 9 10 are based on Paper Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs\n\ntype6:SmithJointApprox used in UE\ntype7:SmithJointCorrelated height_correlated_Smith_G2 used in Google Filament and Frostbite\ntype8:GGX-Smith Correlated Joint Approximate used in Unity HDRP\ntype9:Respawn Entertainment\ntype10:mithJointAniso");
		ImGui::SliderInt("type", &type,1,10);	
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		for (int i = 0; i < 3; i++)
			lightPos[i] = LPos[i];

		ImGui::End();
	}
};
#endif