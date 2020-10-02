#ifndef BLOOMBLUR_H
#define BLOOMBLUR_H
#include<direct.h>
#include"RenderLoop.h"
class Bloom_Blur:public RenderLoop
{
public:
	Bloom_Blur() {
		initDemo();
	};

	Shader *shaderFirst;	
	Shader *shaderLight;
	Shader *shaderBlur;
	Shader *shaderBloomFinal;

	unsigned int containerTexture;
	unsigned int colorBuffers[2];
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	unsigned int hdrFBO;

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	bool bloom = true;
	float exposure = 1.0f;

	RenderedCube *renderCube;
	RenderedScreenFilledQuad *renderQuad;

	glm::vec3 lightPos;
	void SettingBeforLoop()
	{
		renderCube = new RenderedCube();
		renderQuad = new RenderedScreenFilledQuad();

		shaderFirst =new Shader("../../Core/Bloom/bloom.vert", "../../Core/Bloom/bloom.frag");
		shaderLight =new Shader("../../Core/Bloom/bloom.vert", "../../Core/Bloom/light_box.frag");
		shaderBlur =new Shader("../../Core/Bloom/blur.vert", "../../Core/Bloom/blur.frag");
		shaderBloomFinal =new Shader("../../Core/Bloom/bloom_final.vert", "../../Core/Bloom/bloom_final.frag");
		
		containerTexture = loadTexture("../../Core/PublicResource/container.jpg");

		// configure (floating point) framebuffers
		// ---------------------------------------
		
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
		
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
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ping-pong-framebuffer for blurring
		
		
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorbuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}

		// lighting info
		// -------------
		// positions	
		lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
		lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
		lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
		lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
		// colors		
		lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
		lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
		lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
		lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));


		// shader configuration
		// --------------------
		shaderFirst->use();
		shaderFirst->setInt("diffuseTexture", 0);
		shaderBlur->use();
		shaderBlur->setInt("image", 0);
		shaderBloomFinal->use();
		shaderBloomFinal->setInt("scene", 0);
		shaderBloomFinal->setInt("bloomBlur", 1);


		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void initDemo()
	{
		SetDemoName("Bloom_Blur");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}

	void update()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shaderFirst->use();
		shaderFirst->setMat4("projection", projection);
		shaderFirst->setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture);
		// set lighting uniforms
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shaderFirst->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			shaderFirst->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		shaderFirst->setVec3("viewPos", camera.Position);

		renderScene(shaderFirst);
		// finally show all the light sources as bright cubes
		shaderLight->use();
		shaderLight->setMat4("projection", projection);
		shaderLight->setMat4("view", view);

		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.25f));
			shaderLight->setMat4("model", model);
			shaderLight->setVec3("lightColor", lightColors[i]);
			renderCube->drawObject();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. blur bright fragments with two-pass Gaussian Blur 
		// --------------------------------------------------
		bool horizontal = true, first_iteration = true;
		unsigned int amount = 10;
		shaderBlur->use();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur->setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			renderQuad->drawObject();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderBloomFinal->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		shaderBloomFinal->setBool("bloom", bloom);
		shaderBloomFinal->setFloat("exposure", exposure);
		renderQuad->drawObject();
	}
	void renderScene(Shader *shader)
	{
		glm::mat4 model;
		// create one large cube that acts as the floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();
		// then create multiple cubes as the scenery
		glBindTexture(GL_TEXTURE_2D, containerTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
		model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(1.25));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
		model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderFirst->setMat4("model", model);
		renderCube->drawObject();
	}
	void exit()
	{
		shaderFirst->exit();	
		shaderLight->exit();
		shaderBlur->exit();
		shaderBloomFinal->exit();

		delete shaderFirst;	
		delete shaderLight;
		delete shaderBlur;
		delete shaderBloomFinal;

		glDeleteTextures(1, &containerTexture);
		glDeleteTextures(2, colorBuffers);
		glDeleteTextures(2, pingpongColorbuffers);

		glDeleteFramebuffers(2, pingpongFBO);
		glDeleteFramebuffers(1, &hdrFBO);

		lightPositions.clear();
		lightColors.clear();

		renderCube->exit();
		renderQuad->exit();

		delete renderQuad;
		delete renderCube;
	}
	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("Bloom_Blur", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("Bloom", &bloom);
		ImGui::SliderFloat("exposure", &exposure, 0, 2.0);
		ImGui::End();
	}
};

#endif