#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H
#include<direct.h>
#include"RenderLoop.h"
class MotionBlur:public RenderLoop
{
public:
	MotionBlur() {
		initDemo();
	};

	Shader *originalShader;
	Shader *finalShader;
	Shader *tileMaxShader;
	Shader *neigMaxShader;

	RenderedSphere *renderSphere;
	RenderedCube *renderCube;
	RenderedQuad *renderQuad;
	RenderedScreenFilledQuad *renderScreenQuad;

	GLuint woodTexture;
	GLuint cubeTexture;

	GLuint tileMaxFBO;
	GLuint tileMaxTex;
	GLuint neigMaxFBO;
	GLuint neigMaxTex;

	glm::vec3 lightPos;

	glm::mat4 modelMatLast[9];
	glm::mat4 lastView;
	float lastTime;

	unsigned int originalFBO;
	unsigned int colorBuffers[2];

	bool canMove[9];

	int tile = 15;
	float SOFT_Z_EXTENT = 5;
	void initDemo()
	{
		SetDemoName("MotionBlur");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}
	//Paper:A Reconstruction Filter for Plausible Motion Blur
	void SettingBeforLoop()
	{		
		originalShader =new Shader("../../Core/MotionBlur/originalShader.vert", "../../Core/MotionBlur/originalShader.frag");
		finalShader =new Shader("../../Core/MotionBlur/finalShader.vert", "../../Core/MotionBlur/finalShader.frag");
		tileMaxShader =new Shader("../../Core/MotionBlur/finalShader.vert", "../../Core/MotionBlur/tileMax.frag");
		neigMaxShader =new Shader("../../Core/MotionBlur/finalShader.vert", "../../Core/MotionBlur/neigMax.frag");


		woodTexture = loadTexture("../../Core/PublicResource/container.jpg");
		cubeTexture = loadTexture("../../Core/PublicResource/rectangle.jpg");

		renderSphere = new RenderedSphere();
		renderCube = new RenderedCube();
		renderQuad = new RenderedQuad();
		renderScreenQuad = new RenderedScreenFilledQuad();

		glGenFramebuffers(1, &originalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, originalFBO);

		glGenTextures(2, colorBuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}
		// create and attach depth buffer (renderbuffer)
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, attachments);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenFramebuffers(1, &tileMaxFBO);
		glGenTextures(1, &tileMaxTex);
		glBindFramebuffer(GL_FRAMEBUFFER,tileMaxFBO);
		glBindTexture(GL_TEXTURE_2D, tileMaxTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH/tile,SCR_HEIGHT/tile, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tileMaxTex, 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glGenFramebuffers(1, &neigMaxFBO);
		glGenTextures(1, &neigMaxTex);
		glBindFramebuffer(GL_FRAMEBUFFER,neigMaxFBO);
		glBindTexture(GL_TEXTURE_2D, neigMaxTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH/tile,SCR_HEIGHT/tile, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, neigMaxTex, 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		for (int i = 0; i < 9; i++)
		{
			modelMatLast[i] = glm::mat4(1.0);
			canMove[i] = true;
		}
		lastView = glm::mat4(1.0);

		originalShader->use();
		originalShader->setInt("sampleTexture", 0);

		finalShader->use();
		finalShader->setInt("colorTex", 0);
		finalShader->setInt("velocityAndDepth", 1);
		finalShader->setInt("neigMaxTex", 2);

		tileMaxShader->use();
		tileMaxShader->setInt("velocityAndDepth", 0);

		neigMaxShader->use();
		neigMaxShader->setInt("tileMax", 0);
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, originalFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		originalShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		originalShader->setMat4("view", view);
		originalShader->setMat4("projection", projection);
		originalShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		originalShader->setVec3("lightPos", lightPos);
		originalShader->setVec3("viewPos", camera.Position);

		renderScene(originalShader);	
		lastView = view;
		
		glViewport(0, 0, SCR_WIDTH/tile, SCR_HEIGHT/tile);
		glBindFramebuffer(GL_FRAMEBUFFER, tileMaxFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tileMaxShader->use();
		tileMaxShader->setInt("tile", tile);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
		renderScreenQuad->drawObject();

		glBindFramebuffer(GL_FRAMEBUFFER, neigMaxFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		neigMaxShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tileMaxTex);
		renderScreenQuad->drawObject();

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		finalShader->use();
		finalShader->setInt("sampleNum", 15);
		finalShader->setFloat("SOFT_Z_EXTENT", SOFT_Z_EXTENT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, neigMaxTex);
		renderScreenQuad->drawObject();
	}

	void renderScene(Shader *shader)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);

		int i = 0;
		float time = glfwGetTime();
		float deltaTime = time - lastTime;

		lastTime = time;

		shader->setFloat("deltaTime", deltaTime);

		glm::mat4 model = glm::mat4(1.0f);

		//1
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(sin(time*canMove[i]*2)*5, -2.50f, -5.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));		
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderCube->drawObject();

		//2
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(time*canMove[i]*80), glm::vec3(0, 1,0));
		model = glm::translate(model, glm::vec3(0.0f, -2.50f, -7.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));		
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderCube->drawObject();
		
		//3
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((cos(time*canMove[i]*4)+1)*3, (sin(time*canMove[i]*4)+1)*2, -9.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));		
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderCube->drawObject();
		
		//4
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-(sin(time*4)+1)*3, (cos(time*4)+1)*2, -9.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));		
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderCube->drawObject();
		
		//5
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, -1.5, -7.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));		
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderCube->drawObject();

		//6
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-time*160), glm::vec3(0, 1,0));
		model = glm::translate(model, glm::vec3(0.0f, -2.50f, -13.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));		
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderCube->drawObject();

		glBindTexture(GL_TEXTURE_2D, woodTexture);

		//7
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((sin(time*canMove[i]*4)+1)*10, -3.5f, 0.0f));
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderQuad->drawObject();

		//8
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0,0));
		model = glm::translate(model, glm::vec3(0, -20, 0.0f));
		shader->setMat4("model", model);
		shader->setMat4("preModelView", lastView*modelMatLast[i]);  modelMatLast[i] = model; i++;
		renderQuad->drawObject();
	}

	void exit()
	{
		glDeleteFramebuffers(1, &tileMaxFBO);
		glDeleteTextures(1, &tileMaxTex);

		glDeleteFramebuffers(1, &neigMaxFBO);
		glDeleteTextures(1, &neigMaxTex);

		glDeleteFramebuffers(1, &originalFBO);
		glDeleteTextures(2, colorBuffers);
		glDeleteTextures(1, &woodTexture);

		originalShader->exit();
		finalShader->exit();
		tileMaxShader->exit();
		neigMaxShader->exit();

		renderSphere->exit();
		renderQuad->exit();
		renderCube->exit();
		renderScreenQuad->exit();

		delete originalShader;
		delete finalShader;
		delete tileMaxShader;
		delete neigMaxShader;

		delete renderSphere;
		delete renderCube;
		delete renderQuad;
		delete renderScreenQuad;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("MotionBlur", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat("SOFT_Z_EXTENT", &SOFT_Z_EXTENT, 0, 10);
		ImGui::Checkbox("CubeMoveOnX", &canMove[0]);
		ImGui::Checkbox("CubeMoveOnXZ", &canMove[1]);
		ImGui::Checkbox("CubeMoveOnXY", &canMove[2]);
		ImGui::Text("CubeDon'tMove");	
		ImGui::Checkbox("MovePlane", &canMove[6]);
		ImGui::End();
	}
};
#endif