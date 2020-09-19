
#ifndef XTOOAERIALPERSPECTIVE_H
#define XTOONAERIALPESPECTIVE_H
#include<direct.h>
#include"RenderLoop.h"
class XToon_AerialPerspective:public RenderLoop
{
public:
	XToon_AerialPerspective() {
		initDemo();
	};
	Shader *AerialPerspectiveShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *mount;

	glm::vec3 lightPos;


	GLuint aerialPerspective_b;
	GLuint aerialPerspective_c;
	GLuint aerialPerspective_e;
	GLuint aerialPerspective_f;

	int backtLightType = 1;

	float LPos[3];

	float zmin = 2;
	float zmax = 15;
	void initDemo()
	{
		SetDemoName("XToon_AerialPerspective");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		for (int i = 0; i < 3; i++)LPos[i] = lightPos[i];
	}

	void SettingBeforLoop()
	{			
		AerialPerspectiveShader = new Shader("../../Core/XToon/XToon.vert", "../../Core/XToon/XToon_depth.frag");

		mount = new Model("../../Core/XToon/mount.obj");

		//分别对应原论文的b c e f结果
		aerialPerspective_b = loadTextureLinear("../../Core/XToon/aerialPerspective_b.bmp");
		aerialPerspective_c = loadTextureLinear("../../Core/XToon/aerialPerspective_c.bmp");
		aerialPerspective_e = loadTextureLinear("../../Core/XToon/aerialPerspective_e.bmp");
		aerialPerspective_f = loadTextureLinear("../../Core/XToon/aerialPerspective_f.bmp");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();

		AerialPerspectiveShader->use();
		AerialPerspectiveShader->setInt("texsample", 0);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);//齐马蓝


		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		AerialPerspectiveShader->use();
		AerialPerspectiveShader->setMat4("projection", projection);
		AerialPerspectiveShader->setMat4("view", view);

		AerialPerspectiveShader->setVec3("lightPos", lightPos);
		AerialPerspectiveShader->setFloat("zmin", zmin);
		AerialPerspectiveShader->setFloat("zmax", zmax);
		glActiveTexture(GL_TEXTURE0);
		switch (backtLightType)
		{
		case 1:
			glBindTexture(GL_TEXTURE_2D, aerialPerspective_b);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, aerialPerspective_c);
			break;
		case 3:
			glBindTexture(GL_TEXTURE_2D, aerialPerspective_e);
			break;
		case 4:
			glBindTexture(GL_TEXTURE_2D, aerialPerspective_f);
			break;
		default:
			break;
		}	
		renderScene(AerialPerspectiveShader);
	}
	void renderScene(Shader *shader)
	{		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -30.0f, -20.0f));
		model = glm::scale(model, glm::vec3(0.8,0.6f,0.8));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1, 0, 0));
		shader->setMat4("model", model);
		mount->Draw(*shader);
	}
	void exit()
	{
		glDeleteTextures(1, &aerialPerspective_b);
		glDeleteTextures(1, &aerialPerspective_c);
		glDeleteTextures(1, &aerialPerspective_e);
		glDeleteTextures(1, &aerialPerspective_f);

		renderCube->exit();
		renderSphere->exit();
		mount->exit();
		AerialPerspectiveShader->exit();

		delete renderCube;
		delete renderSphere;
		delete mount;
		delete AerialPerspectiveShader;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("XToon_AerialPerspective", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::SliderFloat("zmin", &zmin,0.0,5);
		ImGui::SliderFloat("zmax", &zmax,0,100);
		ImGui::SliderInt("AerialPerspectiveType", &backtLightType, 1, 4);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif