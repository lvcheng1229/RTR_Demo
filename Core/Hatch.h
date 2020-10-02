
#ifndef HATCH_H
#define HATCH_H
#include<direct.h>
#include"RenderLoop.h"
class Hatch:public RenderLoop
{
public:
	Hatch() {
		initDemo();
	};
	Shader *hatchShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *myModel;

	glm::vec3 lightPos;

	float LPos[3];
	float ambi = 0.1, spec = 0.8, diff = 0.9;

	GLuint strokeTex[6];
	void initDemo()
	{
		SetDemoName("Hatch");
		lightPos = glm::vec3(-2.0f, 0.3f, 0.7f);


		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];
		}
	}

	void SettingBeforLoop()
	{			
		//h ttps://gfx.cs.princeton.edu/proj/hatching/  *presentation*
		//h ttps://www.clicktorelease.com/code/cross-hatching/

		hatchShader = new Shader("../../Core/Hatch/hatch.vert", "../../Core/Hatch/hatch.frag");

		myModel = new Model("../../Core/PublicResource/nanosuit/nanosuit.obj");
		myModel->noTex = true;

		strokeTex[0] = loadTextureLinear("../../Core/Hatch/hatch_0.jpg");
		strokeTex[1] = loadTextureLinear("../../Core/Hatch/hatch_1.jpg");
		strokeTex[2] = loadTextureLinear("../../Core/Hatch/hatch_2.jpg");
		strokeTex[3] = loadTextureLinear("../../Core/Hatch/hatch_3.jpg");
		strokeTex[4] = loadTextureLinear("../../Core/Hatch/hatch_4.jpg");
		strokeTex[5] = loadTextureLinear("../../Core/Hatch/hatch_5.jpg");
			

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();

		hatchShader->use();
		for (int i = 0; i < 6; i++)
			hatchShader->setInt("hatch" + to_string(i + 1), i);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		hatchShader->use();
		hatchShader->setMat4("projection", projection);
		hatchShader->setMat4("view", view);
		hatchShader->setVec3("viewPos", camera.Position);
		hatchShader->setVec3("lightPos", lightPos);


		hatchShader->setFloat("ambiWeight", ambi);
		hatchShader->setFloat("diffWeight", diff);
		hatchShader->setFloat("specWeight", spec);
	
		for (int i = 0; i < 6; i++)
		{
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D, strokeTex[i]);
		}

		renderScene(hatchShader);
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
		model = glm::scale(model, glm::vec3(0.1f));
		shader->setMat4("model", model);
		myModel->Draw(*shader);
	}
	void exit()
	{		
		glDeleteTextures(6, strokeTex);
		hatchShader->exit();

		renderCube->exit();
		renderSphere->exit();
		myModel->exit();

		delete hatchShader;

		delete renderCube;
		delete renderSphere;
		delete myModel;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("Hatch", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::SliderFloat("ambi", &ambi,0,1.0);
		ImGui::SliderFloat("diff", &diff,0.5,1.5);
		ImGui::SliderFloat("spec", &spec,0.5,1.5);
		for (int i = 0; i < 3; i++)lightPos[i] = LPos[i];

		ImGui::End();
	}
};
#endif