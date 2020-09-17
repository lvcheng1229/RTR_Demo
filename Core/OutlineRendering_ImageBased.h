
#ifndef ORIMAGEBASED_H
#define ORIMAGEBASED_H
#include<direct.h>
#include"RenderLoop.h"
class OutlineRendering_ImageBased:public RenderLoop
{
public:
	OutlineRendering_ImageBased() {
		initDemo();
	};
	Shader *imagePrcessShader;
	Shader *gBufferShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;
	RenderedTriangle *renderTriangle;

	Model *bunny;

	glm::vec3 lightPos;

	float LPos[3];
	float EdgeCol[3];

	unsigned int gBuffer;
	unsigned int gColor, gNormal;
	unsigned int depthMap;

	void initDemo()
	{
		SetDemoName("OR_ImageBased");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];

		}
		EdgeCol[0] = 1;EdgeCol[1] = 0;EdgeCol[2] = 0;
	}

	void SettingBeforLoop()
	{			
		imagePrcessShader = new Shader("../..//Core/OutlineRendering/imageProcessShader.vert", "../..//Core/OutlineRendering/imageProcessShader.frag", "../..//Core/OutlineRendering/imageProcessShader.geom");
		gBufferShader = new Shader("../..//Core/OutlineRendering/orImageBased.vert", "../..//Core/OutlineRendering/orImageBased.frag");
		bunny = new Model("../..//Core/PublicResource/bunny.obj");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// position color buffer
		glGenTextures(1, &gColor);
		glBindTexture(GL_TEXTURE_2D, gColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColor, 0);
		// normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depthMap, 0);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();
		renderTriangle = new RenderedTriangle();

		imagePrcessShader -> use();
		imagePrcessShader->setInt("gColor", 0);
		imagePrcessShader->setInt("gNormal", 1);
		imagePrcessShader->setInt("depthTex", 2);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);//齐马蓝

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		gBufferShader->use();
		gBufferShader->setMat4("projection", projection);
		gBufferShader->setMat4("view", view);

		gBufferShader->setVec3("viewPos", camera.Position);
		gBufferShader->setVec3("lightPos", lightPos);
		gBufferShader->setVec3("lightcolor", glm::vec3(1, 1, 1));
		gBufferShader->setVec3("objColor", glm::vec3(0.8, 0.8, 0.2));
		renderScene(gBufferShader);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//此处使用了几何着色器来绘制全屏的矩形
		imagePrcessShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gColor);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		imagePrcessShader->setVec3("edgeColor", glm::vec3(EdgeCol[0], EdgeCol[1], EdgeCol[2]));
		renderTriangle->drawObject();

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
		glDeleteBuffers(1, &gBuffer);
		glDeleteTextures(1, &depthMap);
		glDeleteTextures(1, &gColor);
		glDeleteTextures(1, &gNormal);

		renderCube->exit();
		renderSphere->exit();

		imagePrcessShader->exit();
		bunny->exit();
		gBufferShader->exit();

		delete renderCube;
		delete renderSphere;

		delete imagePrcessShader;
		delete bunny;
		delete gBufferShader;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("OR_ImageBased", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::ColorEdit3("EdgeColor", EdgeCol);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif