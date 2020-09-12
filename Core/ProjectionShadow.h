#ifndef PROJECTIONSHADOW_H
#define PROJECTIONSHADOW_H
#include<direct.h>
#include"RenderLoop.h"
class ProjectionShadow:public RenderLoop
{
public:
	ProjectionShadow() {
		initDemo();
	};

	Shader *commonShader;
	Shader *shadowShader;

	RenderedCube *renderedCube;
	RenderedQuad *renderedQuad;
	GLuint woodTexture;
	glm::vec3 lightPos;

	float RoateAngle[3];
	float TransPos[3];
	float LPos[3];
	void initDemo()
	{
		SetDemoName("ProjectionShadow");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);

		for (int i = 0; i < 3; i++)
		{
			RoateAngle[i] = 0;
			TransPos[i] = 0;
			LPos[i] = 5;
		}
		TransPos[1] = -4;			
	}

	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/PublicResource/commonshader.vs", "../../Core/PublicResource/commonshader.fs");
		shadowShader =new Shader("../../Core/ProjectionShadow/emptyShader.vert", "../../Core/ProjectionShadow/emptyShader.frag");

		woodTexture = loadTexture("../../Core/PublicResource/container.jpg");

		commonShader->use();
		commonShader->setInt("sampleTexture", 0);

		renderedCube = new RenderedCube();
		renderedQuad = new RenderedQuad();

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
		
		commonShader->setMat4("view", view);
		commonShader->setMat4("projection", projection);
		commonShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		commonShader->setVec3("lightPos", lightPos);
		commonShader->setVec3("viewPos", camera.Position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25, 0.25, 0.25));
		commonShader->setMat4("model", model);
		renderedCube->drawObject();

		glm::vec3 transPos = glm::vec3(TransPos[0], TransPos[1], TransPos[2]);
		model = glm::mat4(1.0f);
		model = glm::translate(model, transPos);

		model = glm::rotate(model,glm::radians(RoateAngle[0]), glm::vec3(1, 0, 0));
		model = glm::rotate(model,glm::radians(RoateAngle[1]), glm::vec3(0, 1, 0));
		model = glm::rotate(model,glm::radians(RoateAngle[2]), glm::vec3(0, 0, 1));

		commonShader->setMat4("model", model);
		renderedQuad->drawObject();
		
		glm::vec3 norm=glm::normalize(glm::mat3(glm::transpose(glm::inverse(model)))*glm::vec3(0,1,0));
		glm::vec3 worldPos=glm::vec3(model*glm::vec4(0,0,0,1.0));
		
		//将worldPos向法线方向移动一点，以防止出现闪烁现象
		float d = -glm::dot(worldPos+norm*0.001f, norm);

		glm::mat4 projectToPlane = {
			glm::dot(norm,lightPos)+d-lightPos.x*norm.x,-lightPos.x*norm.y,-lightPos.x*norm.z,-lightPos.x*d,
			-lightPos.y*norm.x,glm::dot(norm,lightPos)+d-lightPos.y*norm.y,-lightPos.y*norm.z,-lightPos.y*d,
			-lightPos.z*norm.x,-lightPos.z*norm.y,glm::dot(norm,lightPos)+d-lightPos.z*norm.z,-lightPos.z*d,
			-norm.x,-norm.y,-norm.z,glm::dot(norm,lightPos)
		};
		projectToPlane = glm::transpose(projectToPlane);
		model = glm::mat4(1.0f);
		model *= projectToPlane;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25, 0.25, 0.25));
		
		shadowShader->use();
		shadowShader->setMat4("view", view);
		shadowShader->setMat4("projection", projection);
		shadowShader->setMat4("model", model);

		renderedCube->drawObject();

	}

	void exit()
	{
		glDeleteTextures(1, &woodTexture);
		commonShader->exit();
		shadowShader->exit();

		renderedCube->exit();
		renderedQuad->exit();

		delete commonShader;
		delete shadowShader;
		
		delete renderedCube;
		delete renderedQuad;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("ProjectionShadow", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("RotateAngle",  RoateAngle, -30.0, 30.0);
		ImGui::SliderFloat3("TranslatePlane",  TransPos, -4.0, 4.0);
		ImGui::SliderFloat3("LightPos",  LPos, -5.0, 5.0);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}
		ImGui::End();
	}
};
#endif