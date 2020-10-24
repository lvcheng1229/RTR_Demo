#ifndef SSAODEPTH_H
#define SSAODEPTH_H
#include<direct.h>
#include"RenderLoop.h"
#include <random>    
class SSAO_Depth_Only:public RenderLoop
{
public:
	SSAO_Depth_Only() {
		initDemo();
	};
	glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

	RenderedSphere *sphereObj;
	RenderedCube *renderCube;
	RenderedScreenFilledQuad *renderScreenQuad;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal;
	unsigned int gDepth;
	unsigned int rboDepth;
	unsigned int ssaoFBO, ssaoBlurFBO;
	unsigned int noiseTexture; 
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

	std::vector<glm::vec3> ssaoKernel;

	Model *mymodel;

	Shader *shaderGeometryPass;
	Shader *shaderLightingPass;
	Shader *shaderSSAO;
	Shader *shaderSSAOBlur;

	bool showAo = false;
	void initDemo()
	{
		SetDemoName("SSAO_Depth_Only");
		ssaoKernel.clear();
	}

	void SettingBeforLoop()
	{		
		sphereObj = new RenderedSphere();
		renderCube = new RenderedCube();
		renderScreenQuad = new RenderedScreenFilledQuad();

		glEnable(GL_DEPTH_TEST);

		// build and compile shaders
		// -------------------------
		shaderGeometryPass = new Shader("../../Core/AO/ssao_geometry.vert", "../../Core/AO/ssao_geometry.frag");
		shaderLightingPass = new Shader("../../Core/AO/ssao.vert", "../../Core/AO/ssao_lighting.frag");
		shaderSSAO = new Shader("../../Core/AO/ssao_depth_only.vert", "../../Core/AO/ssao_depth_only.frag");
		shaderSSAOBlur = new Shader("../../Core/AO/ssao.vert", "../../Core/AO/ssao_blur.frag");

		// load models
		// -----------
		mymodel = new Model("../../Core/PublicResource/nanosuit/nanosuit.obj");

		// configure g-buffer framebuffer
		// ------------------------------

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glGenTextures(1, &gDepth);
		glBindTexture(GL_TEXTURE_2D, gDepth);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);
		// position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		// normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		// color + specular color buffer
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, attachments);
		// create and attach depth buffer (renderbuffer)
		


		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// also create framebuffer to hold SSAO processing stage 
		// -----------------------------------------------------

		glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

		// SSAO color buffer
		glGenTextures(1, &ssaoColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Framebuffer not complete!" << std::endl;
		// and blur stage
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glGenTextures(1, &ssaoColorBufferBlur);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// generate sample kernel
		// ----------------------
		std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
		std::default_random_engine generator;

		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0;

			// scale samples s.t. they're more aligned to center of kernel
			//scale = lerp(0.1f, 1.0f, scale * scale);
			scale = 0.1f + scale * scale * (1.0f - 0.1f);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		// generate noise texture
		// ----------------------
		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			float angle = randomFloats(generator)*3.1415926535*0.25;
			float costheta = cos(angle);
			float sintheta = sin(angle);
			glm::vec3 noise(costheta,sintheta, 0.0f); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back(noise);
		}
		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// shader configuration
		// --------------------
		shaderLightingPass->use();
		shaderLightingPass->setInt("gPosition", 0);
		shaderLightingPass->setInt("gNormal", 1);
		shaderLightingPass->setInt("ssao", 3);
		shaderSSAO->use();
		shaderSSAO->setInt("gPosition", 0);
		shaderSSAO->setInt("gNormal", 1);
		shaderSSAO->setInt("texNoise", 2);
		shaderSSAOBlur->use();
		shaderSSAOBlur->setInt("ssaoInput", 0);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
		
		float tanfov2 = tan(glm::radians(camera.Zoom) / 2);
		float ar = (float)SCR_WIDTH / (float)SCR_HEIGHT;
		float n = 0.1, f = 50;
		cout << 1.0 / (ar*tanfov2) << endl;
		cout << 1.0 / (tanfov2) << endl;
		cout << (-n - f) / (n - f) << endl;
		cout << 2 * f*n / (n - f) << endl;
		
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				cout << projection[i][j] << " ";
			}
			cout << endl;
		}
	}

	void update()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shaderGeometryPass->use();
		shaderGeometryPass->setMat4("projection", projection);
		shaderGeometryPass->setMat4("view", view);
		// room cube
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
		shaderGeometryPass->setMat4("model", model);
		shaderGeometryPass->setInt("invertedNormals", 1); // invert normals as we're inside the cube
		renderCube->drawObject();
		shaderGeometryPass->setInt("invertedNormals", 0); 
		//model on the floor
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0, 5.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderGeometryPass->setMat4("model", model);
		mymodel->Draw(*shaderGeometryPass);
		//renderCube->drawObject();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 2. generate SSAO texture
		// ------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderSSAO->use();
		// Send kernel + rotation 
		for (unsigned int i = 0; i < 64; ++i)
			shaderSSAO->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		shaderSSAO->setMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		renderScreenQuad->drawObject();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. blur SSAO texture to remove noise
		// ------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderSSAOBlur->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		renderScreenQuad->drawObject();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
		// -----------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderLightingPass->use();
		// send light relevant uniforms
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
		shaderLightingPass->setVec3("light.Position", lightPosView);
		shaderLightingPass->setVec3("light.Color", lightColor);
		// Update attenuation parameters
		const float linear    = 0.09;
		const float quadratic = 0.032;
		shaderLightingPass->setFloat("light.Linear", linear);
		shaderLightingPass->setFloat("light.Quadratic", quadratic);
		shaderLightingPass->setBool("showAo", showAo);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		renderScreenQuad->drawObject();
	}

	void exit()
	{
		sphereObj->exit();
		renderCube->exit();
		renderScreenQuad->exit();

		delete sphereObj;
		delete renderCube;
		delete renderScreenQuad;

		ssaoKernel.clear();

		mymodel->exit();

		shaderGeometryPass->exit();
		shaderLightingPass->exit();
		shaderSSAO->exit();
		shaderSSAOBlur->exit();

		delete mymodel;

		delete shaderGeometryPass;
		delete shaderLightingPass;
		delete shaderSSAO;
		delete shaderSSAOBlur;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("SSAO_Depth_Only", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("showAo", &showAo);
		ImGui::End();
	}
};
#endif