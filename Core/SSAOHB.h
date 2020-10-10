#ifndef SSAOHB_H
#define SSAOHB_H
#include<direct.h>
#include"RenderLoop.h"
#include <random>    
class SSAOHB:public RenderLoop
{
public:
	SSAOHB() {
		initDemo();
	};
	glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

	RenderedSphere *sphereObj;
	RenderedCube *renderCube;
	RenderedScreenFilledQuad *renderScreenQuad;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedo;
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

	const float  NUM_DIRECTIONS = 8;
	float bias = 0.3;
	float R = 2.0f;

	bool showAo=false;
	void initDemo()
	{
		SetDemoName("SSAOHB");
		ssaoKernel.clear();
	}

	void SettingBeforLoop()
	{		
		//线性深度 见LearnOpenGL 深度测试

		//
		//最早的SSAO采样时不看遮挡关系，直接用深度进行比较
		
		//08年的HBAO则是将深度图看成一个连续的高度场，
		//在一个方向上进行采样时，寻找遮挡贡献最大的点，也就是和法平面夹角最大的点，记为sin(h(theta))
		
		//同时计算法平面和视线的夹角，记为sin(t(theta)),以两者相减的结果进行后续计算
		//详情见此篇论文Image-Space Horizon-Based Ambient Occlusion

		//还有两个小的问题，分别是Bias和衰减的操作
		//可以参考这篇文章https://zhuanlan.zhihu.com/p/103683536

		//这里实现的Demo有几点不同
		//1.HBAO以某方向上夹角最大的点作为有贡献的点，而此demo将某方向上所有点都算作贡献点
		//2.原来的HBAO是通过uv和深度值恢复出来在相机空间的Position和Normal
		//	但我实现的时候总是有点问题，所以就改了一下，又加了两个新的纹理来存储Position和Normal，以便用于后续的计算

		sphereObj = new RenderedSphere();
		renderCube = new RenderedCube();
		renderScreenQuad = new RenderedScreenFilledQuad();

		glEnable(GL_DEPTH_TEST);

		// build and compile shaders
		// -------------------------
		shaderGeometryPass = new Shader("../../Core/AO/ssao_geometry.vert", "../../Core/AO/ssaohb_geometry.frag");
		shaderLightingPass = new Shader("../../Core/AO/ssao.vert", "../../Core/AO/ssao_lighting.frag");
		shaderSSAO = new Shader("../../Core/AO/ssao.vert", "../../Core/AO/ssaohb.frag");
		shaderSSAOBlur = new Shader("../../Core/AO/ssao.vert", "../../Core/AO/ssao_blur.frag");

		// load models
		// -----------
		mymodel = new Model("../../Core/PublicResource/nanosuit/nanosuit.obj");

		// configure g-buffer framebuffer
		// ------------------------------

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

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
		glGenTextures(1, &gAlbedo);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
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

		// generate noise texture
		// ----------------------
		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{		
			float rand1 = randomFloats(generator);
			float rand2 = randomFloats(generator);

			float Angle = 2.f * 3.1415926535 * rand1 / NUM_DIRECTIONS;

			glm::vec3 noise(rand1, rand2,0.0); 
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
		shaderLightingPass->setInt("gAlbedo", 2);
		shaderLightingPass->setInt("ssao", 3);
		shaderSSAO->use();
		//shaderSSAO->setInt("gPosition", 0);
		//shaderSSAO->setInt("texNoise", 1);
		shaderSSAO->setInt("gPosition", 0);
		shaderSSAO->setInt("gNormal", 1);
		shaderSSAO->setInt("texNoise", 2);
		shaderSSAOBlur->use();
		shaderSSAOBlur->setInt("ssaoInput", 0);

	}

	void update()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float FOV = glm::radians(camera.Zoom);
		float Aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
		glm::mat4 projection = glm::perspective(FOV, Aspect, 0.1f, 50.0f);
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
		shaderSSAO->setVec2("InvFullResolution", glm::vec2(1.0 / SCR_WIDTH, 1.0 / SCR_HEIGHT));
			
		float projScale = float(SCR_HEIGHT) / (tanf( FOV * 0.5f) * 2.0f);
		
		shaderSSAO->setFloat("NegInvR2", -1.0/(R*R));
		shaderSSAO->setFloat("RadiusToScreen", R*0.5f*projScale);
		
		float NDotVBias = std::min(std::max(0.0f, bias),1.0f);
		float AOMultiplier = 1.0f / (1.0f - NDotVBias);
		shaderSSAO->setFloat("NDotVBias", NDotVBias);
		shaderSSAO->setFloat("AOMultiplier", AOMultiplier);
		shaderSSAO->setFloat("c", 1.0/tanf(FOV*0.5));
		shaderSSAO->setFloat("a", Aspect);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, gPosition);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, noiseTexture);
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
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
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
		ImGui::Begin("SSAOHB", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat("bias", &bias, 0, 0.9999f);
		ImGui::SliderFloat("R", &R, 0, 4.0f);
		ImGui::Checkbox("showAo", &showAo);
		ImGui::End();
	}
};
#endif