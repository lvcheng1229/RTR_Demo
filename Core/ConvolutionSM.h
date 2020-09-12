
#ifndef SONVOLUTIONSM_H
#define SONVOLUTIONSM_H
#include<direct.h>
#include"RenderLoop.h"
class ConvolutionSM:public RenderLoop
{
public:
	ConvolutionSM() {
		initDemo();
	};
	Shader *ShadowMapShader;
	Shader *SecondShader;

	RenderedQuad *renderQuad;
	RenderedCube *renderCube;

	GLuint rectangleTexture;
	GLuint depthMap;
	GLuint depthMapFBO;
	glm::vec3 lightPos;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	void initDemo()
	{
		SetDemoName("ConvolutionSM");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	}

	void SettingBeforLoop()
	{			
		ShadowMapShader =new Shader("../../Core/ConvolutionSM/ShadowMapShader.vert", "../../Core/ConvolutionSM/ShadowMapShader.frag");

		SecondShader =new Shader("../../Core/ConvolutionSM/secondShader.vert", "../../Core/ConvolutionSM/secondShader.frag");

		rectangleTexture = loadTexture("../../Core/PublicResource/rectangle.jpg");
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		SecondShader->use();
		SecondShader->setInt("diffuseTexture", 0);
		SecondShader->setInt("shadowMap", 1);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderQuad = new RenderedQuad();
		renderCube = new RenderedCube();
	}

	void update()
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;

		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(1.0, 0.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		ShadowMapShader->use();
		ShadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rectangleTexture);
		renderScene(ShadowMapShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);

		SecondShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 view = camera.GetViewMatrix();
		SecondShader->setMat4("projection", projection);
		SecondShader->setMat4("view", view);
		// set light uniforms
		SecondShader->setVec3("viewPos", camera.Position);
		SecondShader->setVec3("lightPos", lightPos);
		SecondShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		SecondShader->setFloat("pi", 3.14159265358979323846264338327950288419716939937510582097494459230781);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rectangleTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(SecondShader);
	}
	void renderScene(Shader *shader)
	{
		// floor
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0));
		shader->setMat4("model", model);
		renderQuad->drawObject();
		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -2.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -2.0f, -1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -1.5f, -1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.5f, -4.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();

	}
	void exit()
	{
		glDeleteFramebuffers(1, &depthMapFBO);
		glDeleteTextures(1, &rectangleTexture);
		glDeleteTextures(1, &depthMap);
		ShadowMapShader->exit();
		SecondShader->exit();
		renderQuad->exit();
		renderCube->exit();

		delete ShadowMapShader;
		delete SecondShader;
		delete renderQuad;
		delete renderCube;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("ConvolutionSM", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat("LightPoxX", &lightPos.x, -4.f, 4.f);
		ImGui::SliderFloat("LightPoxY", &lightPos.y, -4.f, 4.f);
		ImGui::SliderFloat("LightPoxZ", &lightPos.z, -4.f, 4.f);
		ImGui::End();
	}
};
#endif