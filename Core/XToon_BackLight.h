
#ifndef XTOONBACKLIGHT_H
#define XTOONBACKLIGHT_H
#include<direct.h>
#include"RenderLoop.h"
class XToon_BackLight:public RenderLoop
{
public:
	XToon_BackLight() {
		initDemo();
	};
	Shader *BackLightShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *bunny;

	glm::vec3 lightPos;


	GLuint backLight_b;
	GLuint backLight_c;
	GLuint backLight_d;

	int backtLightType = 1;

	float LPos[3];

	float factor = 1.0;
	void initDemo()
	{
		SetDemoName("XToon_BackLight");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		for (int i = 0; i < 3; i++)LPos[i] = lightPos[i];
	}

	void SettingBeforLoop()
	{			
		BackLightShader = new Shader("../../Core/XToon/XToon.vert", "../../Core/XToon/XToon_silhouette.frag");

		bunny = new Model("../../Core/PublicResource/bunny.obj");

		//分别对应原论文的 b c d结果
		backLight_b = loadTextureLinear("../../Core/XToon/backLight_b.bmp");
		backLight_c = loadTextureLinear("../../Core/XToon/backLight_c.bmp");
		backLight_d = loadTextureLinear("../../Core/XToon/backLight_d.bmp");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();

		BackLightShader->use();
		BackLightShader->setInt("texsample", 0);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);//齐马蓝


		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		BackLightShader->use();
		BackLightShader->setMat4("projection", projection);
		BackLightShader->setMat4("view", view);
		BackLightShader->setVec3("viewPos", camera.Position);
		BackLightShader->setVec3("lightPos", lightPos);

		BackLightShader->setFloat("s", factor);

		glActiveTexture(GL_TEXTURE0);
		switch (backtLightType)
		{
		case 1:
			glBindTexture(GL_TEXTURE_2D, backLight_b);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, backLight_c);
			break;
		case 3:
			glBindTexture(GL_TEXTURE_2D, backLight_d);
			break;
		default:
			break;
		}	
		renderScene(BackLightShader);
	}
	void renderScene(Shader *shader)
	{		
		glm::mat4 model = glm::mat4(1.0f);

		//bunny
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.3f, -3.0f));
		model = glm::scale(model, glm::vec3(8.0f));
		shader->setMat4("model", model);
		bunny->Draw(*shader);
	}
	void exit()
	{
		glDeleteTextures(1, &backLight_b);
		glDeleteTextures(1, &backLight_c);
		glDeleteTextures(1, &backLight_d);

		renderCube->exit();
		renderSphere->exit();
		bunny->exit();
		BackLightShader->exit();

		delete renderCube;
		delete renderSphere;
		delete bunny;
		delete BackLightShader;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("XToon_BackLight", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::SliderFloat("factor", &factor,0.0,3.0);
		ImGui::SliderInt("backLightType", &backtLightType, 1, 3);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif