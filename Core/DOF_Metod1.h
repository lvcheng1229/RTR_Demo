#ifndef DOFMETOD1_H
#define DOFMETOD1_H
#include<direct.h>
#include"RenderLoop.h"
class DOF_Metod1:public RenderLoop
{
public:
	DOF_Metod1() {
		initDemo();
	};

	Shader *commonShader;	
	Shader *shaderBlur;
	Shader *finalShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;
	RenderedScreenFilledQuad *renderQuad;
	RenderedQuad *renderPlane;

	unsigned int colorBuffers[3];
	unsigned int pingpongFBO1[2];
	unsigned int pingpongColorbuffers1[2];
	unsigned int pingpongFBO2[2];
	unsigned int pingpongColorbuffers2[2];
	unsigned int originalFBO;

	GLuint woodTexture;
	glm::vec3 lightPos;

	float focusDistance = 10.0f;
	float focusRange = 5.0f;
	bool showNearFieldProblem = false;
	void SettingBeforLoop()
	{
		//commonShader =new Shader("../../Core/DOF_Method1/original.vert", "../../Core/DOF_Method1/original.frag");
		//shaderBlur =new Shader("../../Core/Bloom/blur.vert", "../../Core/Bloom/blur.frag");
		//finalShader =new Shader("../../Core/DOF_Method1/final.vert", "../../Core/DOF_Method1/final.frag");
		//
		//woodTexture = loadTextureLinear("../../Core/PublicResource/container.jpg");
		commonShader =new Shader("F:/RTR_Demo/Core/DOF_Method1/original.vert", "F:/RTR_Demo/Core/DOF_Method1/original.frag");
		shaderBlur =new Shader("F:/RTR_Demo/Core/Bloom/blur.vert", "F:/RTR_Demo/Core/Bloom/blur.frag");
		finalShader =new Shader("F:/RTR_Demo/Core/DOF_Method1/final.vert", "F:/RTR_Demo/Core/DOF_Method1/final.frag");

		woodTexture = loadTextureLinear("F:/RTR_Demo/Core/PublicResource/container.jpg");
		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();
		renderQuad = new RenderedScreenFilledQuad();
		renderPlane = new RenderedQuad();

		
		glGenFramebuffers(1, &originalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, originalFBO);

		glGenTextures(3, colorBuffers);
		for (unsigned int i = 0; i < 3; i++)
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
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenFramebuffers(2, pingpongFBO1);
		glGenTextures(2, pingpongColorbuffers1);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO1[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers1[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers1[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}

		glGenFramebuffers(2, pingpongFBO2);
		glGenTextures(2, pingpongColorbuffers2);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO2[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers2[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers2[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}
		commonShader->use();
		commonShader->setInt("sampleTexture", 0);
		shaderBlur->use();
		shaderBlur->setInt("image", 0);
		finalShader->use();
		finalShader->setInt("originalColor", 0);
		finalShader->setInt("NearFieldColor", 1);
		finalShader->setInt("restColor", 2);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void initDemo()
	{
		SetDemoName("DOF_Metod1");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}

	void update()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, originalFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 view = camera.GetViewMatrix();

		commonShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		commonShader->setMat4("view", view);
		commonShader->setMat4("projection", projection);
		commonShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		commonShader->setVec3("lightPos", lightPos);
		commonShader->setVec3("viewPos", camera.Position);
		
		commonShader->setFloat("focusDistance", focusDistance);
		commonShader->setFloat("focusRange", focusRange);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		model = glm::translate(model, glm::vec3(0.0f, -1.50f, -2.0f));
		commonShader->setMat4("model", model);
		renderScene(commonShader);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. blur with two-pass Gaussian Blur 
		// --------------------------------------------------
		glActiveTexture(GL_TEXTURE0);
	
		bool horizontal1 = true, first_iteration = true;
		unsigned int amount = 10;
		shaderBlur->use();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO1[horizontal1]);
			shaderBlur->setBool("horizontal", horizontal1);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers1[!horizontal1]);  // bind texture of other framebuffer (or scene if first iteration)
			renderQuad->drawObject();
			horizontal1 = !horizontal1;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		bool horizontal2 = true; 
		first_iteration = true;
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO2[horizontal2]);
			shaderBlur->setBool("horizontal", horizontal2);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[2] : pingpongColorbuffers2[!horizontal2]);  // bind texture of other framebuffer (or scene if first iteration)
			renderQuad->drawObject();
			horizontal2 = !horizontal2;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		finalShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers1[!horizontal1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers2[!horizontal2]);
		finalShader->setFloat("focusDistance", focusDistance);
		finalShader->setFloat("focusRange", focusRange);
		finalShader->setBool("showNearFieldProblem", showNearFieldProblem);
		renderQuad->drawObject();

	}
	void renderScene(Shader *shader)
	{		
		glm::mat4 model = glm::mat4(1.0f);

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3((i-5)*2, -2.0f, -j*2));
				model = glm::scale(model, glm::vec3(0.5f));
				shader->setMat4("model", model);
				renderCube->drawObject();
			}
		}
		//plane
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, -15.0f));
		shader->setMat4("model", model);
		renderPlane->drawObject();
	}
	void exit()
	{
		glDeleteTextures(1, &woodTexture);
		glDeleteTextures(3, colorBuffers);
		glDeleteTextures(2, pingpongColorbuffers1);
		glDeleteTextures(2, pingpongColorbuffers2);

		glDeleteFramebuffers(2, pingpongFBO1);
		glDeleteFramebuffers(2, pingpongFBO2);
		glDeleteFramebuffers(1, &originalFBO);

		commonShader->exit();
		shaderBlur->exit();
		finalShader->exit();

		renderCube->exit();
		renderSphere->exit();
		renderQuad->exit();

		delete commonShader;
		delete shaderBlur;
		delete finalShader;

		delete renderCube;
		delete renderSphere;
		delete renderQuad;
	}
	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("DOF_Metod1", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat("FocusDistance", &focusDistance,5,20);
		ImGui::SliderFloat("FocusRange", &focusRange,0,10);
		ImGui::Checkbox("showNearFieldProblem", &showNearFieldProblem);
		
		ImGui::End();
	}
};

#endif