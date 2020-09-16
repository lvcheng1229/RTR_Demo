
#ifndef ORNORMALEDGE_H
#define ORNORMALEDGE_H
#include<direct.h>
#include"RenderLoop.h"
class OutlineRendering_NormalEdge:public RenderLoop
{
public:
	OutlineRendering_NormalEdge() {
		initDemo();
	};
	Shader *OR_NormalEdge;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *bunny;

	GLuint containerTexture;
	glm::vec3 lightPos;

	float LPos[3];
	float EdgeCol[3];
	float threshold = 0.2;
	void initDemo()
	{
		SetDemoName("OR_NormalEdge");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		threshold = 0.2;
		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];
		}
		EdgeCol[0] = 0.8235;EdgeCol[1] = 0.47;EdgeCol[2] = 0.47;
	}
	//-----------------------------------------------
	//一个缺点是contour line的宽度是变化的，其宽度取决于表面的曲率
	//例如立方体垂直于视线的那一面会描边失败
	//-----------------------------------------------
	void SettingBeforLoop()
	{			
		containerTexture = loadTexture("../../Core/PublicResource/container.jpg");
		OR_NormalEdge = new Shader("../../Core/OutlineRendering/orNormalEdge.vert", "../../Core/OutlineRendering/orNormalEdge.frag");

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

		OR_NormalEdge->use();
		OR_NormalEdge->setMat4("projection", projection);
		OR_NormalEdge->setMat4("view", view);

		OR_NormalEdge->setVec3("viewPos", camera.Position);
		OR_NormalEdge->setVec3("lightPos", lightPos);
		OR_NormalEdge->setVec3("lightcolor", glm::vec3(1, 1, 1));

		OR_NormalEdge->setVec3("objColor", glm::vec3(0.8, 0.8, 0.2));
		OR_NormalEdge->setVec3("edgeColor", glm::vec3(EdgeCol[0], EdgeCol[1], EdgeCol[2]));
		OR_NormalEdge->setFloat("threshold", threshold);
		renderScene(OR_NormalEdge);
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
		OR_NormalEdge->exit();
		bunny->exit();

		delete renderCube;
		delete renderSphere;
		delete OR_NormalEdge;
		delete bunny;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("OR_NormalEdge", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::SliderFloat("threshold", &threshold, 0.0, 0.8);
		ImGui::ColorEdit3("EdgeColor", EdgeCol);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif