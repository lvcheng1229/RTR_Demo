
#ifndef XTOONHIGHLIGHT_H
#define XTOONHIGHLIGHT_H
#include<direct.h>
#include"RenderLoop.h"
class XToon_HighLight:public RenderLoop
{
public:
	XToon_HighLight() {
		initDemo();
	};
	Shader *HighLightShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *bunny;

	glm::vec3 lightPos;


	GLuint highLight_b;
	GLuint highLight_c;
	GLuint highLight_d;
	
	int hightLightType = 1;

	float LPos[3];

	float shiness = 1.0;
	void initDemo()
	{
		SetDemoName("XToon_HighLight");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		for (int i = 0; i < 3; i++)LPos[i] = lightPos[i];
	}

	void SettingBeforLoop()
	{			
		HighLightShader = new Shader("../../Core/XToon/XToon.vert", "../../Core/XToon/XToon_highlight.frag");

		bunny = new Model("../../Core/PublicResource/bunny.obj");

		//分别对应原论文Figure11的 b c d结果
		highLight_b = loadTextureLinear("../../Core/XToon/highLight_b.bmp");
		highLight_c = loadTextureLinear("../../Core/XToon/highLight_c.bmp");
		highLight_d = loadTextureLinear("../../Core/XToon/highLight_d.bmp");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();
		
		HighLightShader->use();
		HighLightShader->setInt("texsample", 0);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);//齐马蓝


		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		HighLightShader->use();
		HighLightShader->setMat4("projection", projection);
		HighLightShader->setMat4("view", view);
		HighLightShader->setVec3("viewPos", camera.Position);
		HighLightShader->setVec3("lightPos", lightPos);
	
		HighLightShader->setFloat("s", shiness);

		glActiveTexture(GL_TEXTURE0);
		switch (hightLightType)
		{
		case 1:
			glBindTexture(GL_TEXTURE_2D, highLight_b);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, highLight_c);
			break;
		case 3:
			glBindTexture(GL_TEXTURE_2D, highLight_d);
			break;
		default:
			break;
		}	
		renderScene(HighLightShader);
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
		glDeleteTextures(1, &highLight_b);
		glDeleteTextures(1, &highLight_c);
		glDeleteTextures(1, &highLight_d);

		renderCube->exit();
		renderSphere->exit();
		bunny->exit();
		HighLightShader->exit();

		delete renderCube;
		delete renderSphere;
		delete bunny;
		delete HighLightShader;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("XToon_HighLight", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::SliderFloat("shiness", &shiness,1.0,2.0);
		ImGui::SliderInt("hightLightType", &hightLightType, 1, 3);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif