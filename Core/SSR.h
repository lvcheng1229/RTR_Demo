#ifndef SSR_H
#define SSR_H
#include<direct.h>
#include"RenderLoop.h"
class SSR:public RenderLoop
{
public:
	SSR() {
		initDemo();
	};

	Shader *gShader;
	Shader *testShader;
	Shader *ssrShader;

	RenderedSphere *sphereObj;
	RenderedCube *cubeObj;
	RenderedQuad *quadObj;
	RenderedScreenFilledQuad *screenQuad;

	GLuint woodTexture;
	GLuint planeTexture;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gColor, gMask;

	unsigned int ssrBuffer;
	unsigned int ssrTexture;

	glm::vec3 lightPos;

	void initDemo()
	{
		SetDemoName("SSR");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}

	void SettingBeforLoop()
	{		
		//gShader =new Shader("../../Core/SSR/gShader.vert", "../../Core/SSR/gShader.frag");
		//testShader = new Shader("../../Core/SSR/test.vert", "../../Core/SSR/test.frag");
		//
		//woodTexture = loadTexture("../../Core/PublicResource/container.jpg");
		//planeTexture = loadTexture("../../Core/PublicResource/wood.jpg");
		gShader =new Shader("F:/RTR_Demo/Core/SSR/gShader.vert", "F:/RTR_Demo/Core/SSR/gShader.frag");
		ssrShader = new Shader("F:/RTR_Demo/Core/SSR/test.vert", "F:/RTR_Demo/Core/SSR/ssr.frag");
		testShader = new Shader("F:/RTR_Demo/Core/SSR/test.vert", "F:/RTR_Demo/Core/SSR/finalShader.frag");

		woodTexture = loadTexture("F:/RTR_Demo/Core/PublicResource/container.jpg");
		planeTexture = loadTexture("F:/RTR_Demo/Core/PublicResource/wood.jpg");

		gShader->use();
		gShader->setInt("sampleTexture", 0);

		testShader->use();
		testShader->setInt("gColor", 0);
		testShader->setInt("ssrColor", 1);

		ssrShader->use();
		ssrShader->setInt("positionTexture", 0);
		ssrShader->setInt("normalTexture", 1);
		ssrShader->setInt("maskTexture", 2);



		sphereObj = new RenderedSphere();
		cubeObj = new RenderedCube();
		quadObj = new RenderedQuad();
		screenQuad = new RenderedScreenFilledQuad();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		
		glGenTextures(1, &gColor);
		glBindTexture(GL_TEXTURE_2D, gColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColor, 0);

		glGenTextures(1, &gMask);
		glBindTexture(GL_TEXTURE_2D, gMask);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMask, 0);

		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 ,GL_COLOR_ATTACHMENT3};
		glDrawBuffers(4, attachments);
		// create and attach depth buffer (renderbuffer)

		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glGenTextures(1, &ssrTexture);
		glBindTexture(GL_TEXTURE_2D, ssrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenFramebuffers(1, &ssrBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, ssrBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrTexture, 0);

		//MouseMove = false;
	}

	void update()
	{
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 view = camera.GetViewMatrix();
		//glm::mat4 view(1.0);
		
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		gShader->setMat4("view", view);
		gShader->setMat4("projection", projection);
		gShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		gShader->setVec3("lightPos", lightPos);
		gShader->setVec3("viewPos", camera.Position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		gShader->setBool("specMask", false);
		
		//model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(15, 15, 15));
		//gShader->setMat4("model", model);
		//cubeObj->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -6.5f));
		gShader->setMat4("model", model);
		sphereObj->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, -0.5f, -7.0f));
		gShader->setMat4("model", model);
		cubeObj->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, -0.5f, -6.0f));
		gShader->setMat4("model", model);
		cubeObj->drawObject();

		gShader->setBool("specMask", true);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		gShader->setMat4("model", model);
		quadObj->drawObject();

		glBindFramebuffer(GL_FRAMEBUFFER, ssrBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ssrShader->use();
		ssrShader->setMat4("lensProjection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gMask);
		screenQuad->drawObject();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		testShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gColor);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ssrTexture);
		screenQuad->drawObject();
	}

	void exit()
	{
		glDeleteBuffers(1, &gBuffer);

		glDeleteTextures(1, &woodTexture);
		glDeleteTextures(1, &planeTexture);

		glDeleteTextures(1, &gPosition);
		glDeleteTextures(1, &gNormal);
		glDeleteTextures(1, &gColor);
		glDeleteTextures(1, &gMask);

		gShader->exit();
		testShader->exit();
		ssrShader->exit();

		sphereObj->exit();
		cubeObj->exit();
		quadObj->exit();
		screenQuad->exit();

		delete gShader;
		delete testShader;
		delete ssrShader;

		delete sphereObj;
		delete cubeObj;
		delete quadObj;
		delete screenQuad;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("SSR", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Text("This is SSR");
		ImGui::End();
	}
};
#endif