
#ifndef PCF_H
#define PCFP_H
#include<direct.h>
#include"RenderLoop.h"
class PCF:public RenderLoop
{
public:
	PCF() {
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


	float bias;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	int type = 1;
	void initDemo()
	{
		SetDemoName("PCF");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		bias = 0.5;
		type = 1;
	}

	void SettingBeforLoop()
	{			
		ShadowMapShader =new Shader("../../Core/PCF/ShadowMapShader.vert", "../../Core/PCF/ShadowMapShader.frag");

		SecondShader =new Shader("../../Core/PCF/secondShader.vert", "../../Core/PCF/secondShader.frag");

		rectangleTexture = loadTexture("../../Core/PublicResource/rectangle.jpg");
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		SecondShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//glm::mat4 projection = glm::perspective(angle, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 view = camera.GetViewMatrix();
		SecondShader->setMat4("projection", projection);
		SecondShader->setMat4("view", view);
		// set light uniforms
		SecondShader->setVec3("viewPos", camera.Position);
		SecondShader->setVec3("lightPos", lightPos);
		SecondShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		SecondShader->setFloat("offset", bias);
		//1:3x3 grid pattern
		//2:8-tap Posisson sample pattern
		//3:3x3 guassian filter
		//4:random 8 samples
		//5:no pcf
		int sampleNum=9;
		
		glm::vec2 *samplePos;
		float *weight;
		if (type==1)
		{
			glm::vec2 TsamplePos[9] = {
				glm::vec2(-1,1),glm::vec2(0,1),glm::vec2(1,1),
				glm::vec2(-1,0),glm::vec2(0,0),glm::vec2(1,0),
				glm::vec2(-1,-1),glm::vec2(0,1),glm::vec2(1,-1),
			};
			float Tweight[9] = {
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
			};
			samplePos = TsamplePos;
			weight = Tweight;
		}
		else if (type == 2)
		{
			glm::vec2 TsamplePos[9] = {
				glm::vec2(-0.94201624, -0.39906216),glm::vec2(0.94558609, -0.76890725),glm::vec2(-0.094184101, -0.92938870),
				glm::vec2(0.34495938, 0.29387760),glm::vec2(0,0),glm::vec2(0,0),
				glm::vec2(0,0),glm::vec2(0,0),glm::vec2(0,0),
			};
			float Tweight[9] = {
				1.0/(float)4,1.0/(float)4,1.0/(float)4,
				1.0/(float)4,0,0,
				0,0,0,
			};
			samplePos = TsamplePos;
			weight = Tweight;
		}
		else if (type == 3)
		{
			glm::vec2 TsamplePos[9] = {
				glm::vec2(-1,1),glm::vec2(0,1),glm::vec2(1,1),
				glm::vec2(-1,0),glm::vec2(0,0),glm::vec2(1,0),
				glm::vec2(-1,-1),glm::vec2(0,1),glm::vec2(1,-1),
			};
			float Tweight[9] = {
				0.0625,0.125,0.0625,
				0.125,0.25,0.125,
				0.0625,0.125,0.0625,
			};
			samplePos = TsamplePos;
			weight = Tweight;
		}
		else if (type == 4)
		{
			glm::vec2 TsamplePos[9] = {
				glm::vec2(0.000000, 0.000000)*1.5f,glm::vec2(0.079821, 0.165750)*1.5f,glm::vec2(-0.331500, 0.159642)*1.5f,
				glm::vec2(-0.239463, -0.497250)*1.5f,glm::vec2(0.662999, -0.319284)*1.5f,glm::vec2(0.399104, 0.828749)*1.5f,
				glm::vec2(-0.994499, 0.478925)*1.5f,glm::vec2(-0.558746, -1.160249)*1.5f,glm::vec2(0.478925,-0.480140)*1.5f,
			};
			float Tweight[9] = {
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
			};
			samplePos = TsamplePos;
			weight = Tweight;
		}
		else
		{
			glm::vec2 TsamplePos[9] = {
				glm::vec2(0),glm::vec2(0),glm::vec2(0),
				glm::vec2(0),glm::vec2(0),glm::vec2(0),
				glm::vec2(0),glm::vec2(0),glm::vec2(0),
			};
			float Tweight[9] = {
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
				1.0/(float)sampleNum,1.0/(float)sampleNum,1.0/(float)sampleNum,
			};
			samplePos = TsamplePos;
			weight = Tweight;
		}
		//SecondShader->setFloat("samplesNum", sampleNum);
		for (int i = 0; i < sampleNum; i++)
		{
			SecondShader->setVec2("samplePos["+to_string(i)+"]", samplePos[i]);
			SecondShader->setFloat("weight[" + to_string(i) + "]", weight[i]);
		}
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
		ImGui::Begin("PCF", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat("Bias", &bias, 0.f, 1.f);
		ImGui::SliderFloat("LightPoxX", &lightPos.x, -4.f, 4.f);
		ImGui::SliderFloat("LightPoxY", &lightPos.y, -4.f, 4.f);
		ImGui::SliderFloat("LightPoxZ", &lightPos.z, -4.f, 4.f);
		ImGui::Text("1:3x3 grid pattern\n2:4 Posisson samples\n3:3x3 guassian filter\n4:random 9 samples\n5:no pcf ");
		ImGui::SliderInt("Type",&type,1,5);	
		ImGui::End();
	}
};
#endif