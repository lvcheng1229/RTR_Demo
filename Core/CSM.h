
#ifndef CSM_H
#define CSM_H
#include<direct.h>
#include"RenderLoop.h"
class CSM:public RenderLoop
{
public:
	CSM() {
		initDemo();
	};
	Shader *ShadowMapShader;
	Shader *SecondShader;

	RenderedQuad *renderQuad;
	RenderedCube *renderCube;

	GLuint rectangleTexture;
	GLuint depthMap[4];
	GLuint depthMapFBO[4];
	glm::vec3 lightPos;

	const unsigned int N = 4;
	float zfar_i[4 + 1];
	float nearp = 0.1, farp = 30.0;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	void initDemo()
	{
		SetDemoName("CSM");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		
		nearp = 0.1; 
		farp = 30.0;
	}

	void SettingBeforLoop()
	{			
		
		for (int i = 0; i <= N; i++)
		{
			zfar_i[i] = nearp * pow(farp / nearp, (float)i / (float)N);
			cout << zfar_i[i] << endl;
		}

		ShadowMapShader =new Shader("F:/RTR_Demo/Core/CSM/ShadowMapShader.vert", "F:/RTR_Demo/Core/CSM/ShadowMapShader.frag");
		SecondShader =new Shader("F:/RTR_Demo/Core/CSM/secondShader.vert", "F:/RTR_Demo/Core/CSM/secondShader.frag");
		rectangleTexture = loadTexture("F:/RTR_Demo/Core/PublicResource/rectangle.jpg");

		glGenTextures(4, depthMap);
		for (int i = 0; i < 4; i++)
		{
			glBindTexture(GL_TEXTURE_2D, depthMap[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		glGenFramebuffers(4, depthMapFBO);
		for (int i = 0; i < 4; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}



		SecondShader->use();
		SecondShader->setInt("diffuseTexture", 0);
		SecondShader->setInt("shadowMap1", 1);
		SecondShader->setInt("shadowMap2", 2);
		SecondShader->setInt("shadowMap3", 3);
		SecondShader->setInt("shadowMap4", 4);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderQuad = new RenderedQuad();
		renderCube = new RenderedCube();
	}

	void update()
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//构建视锥体
		glm::vec3 frustumPos[5][4];		
		for (int i = 0; i <= N; i++)
		{
			float tempD = zfar_i[i];
			float hd2 = tempD * tan(camera.Zoom / 2.0*3.1415926535 / 180.0);

			glm::vec3 wd2vec = glm::normalize(camera.Right)*hd2*(float)SCR_WIDTH / (float)SCR_HEIGHT;
			glm::vec3 hd2vec = glm::normalize(camera.Up)*hd2;
			glm::vec3 d = camera.Position + glm::normalize(camera.Front)*tempD;

			frustumPos[i][0] = d + wd2vec + hd2vec;
			frustumPos[i][1] = d + wd2vec - hd2vec;
			frustumPos[i][2] = d - wd2vec + hd2vec;
			frustumPos[i][3] = d - wd2vec - hd2vec;
		}

		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(1.0, 0.0, 0.0));
		
		
		for (int i = 0; i <= N; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				//ph=PMp P=I
				frustumPos[i][j] = glm::vec3(lightView*glm::vec4(frustumPos[i][j], 1.0));
			}
		}

		//P=CPz
		glm::mat4 PMat[4];
		for (int i = 0; i < N; i++)
		{
			glm::vec3 tempVec;
			glm::vec3 minp(1000, 1000, 1000);
			glm::vec3 maxp(-1000, -1000, -1000);
			for (int j = 0; j < 4; j++)
			{
				for (int l = 0; l <= 1; l++)
				{
					tempVec = frustumPos[i+l][j];
					for (int k = 0; k < 3; k++)
					{
						if (tempVec[k] < minp[k])minp[k] = tempVec[k];
						if (tempVec[k] > minp[k])maxp[k] = tempVec[k];
					}
				}
			}
			for (int k = 0; k < 3; k++)
			{
				if (minp[k] == 1000)cout << "erro\n";
				if (maxp[k] == -1000)cout << "erro\n";
			}
			float Sx = 2.0 / (maxp[0] - minp[0]);
			float Sy = 2.0 / (maxp[1] - minp[1]);
			float Ox = -0.5*(maxp[0] + minp[0])*Sx;
			float Oy = -0.5*(maxp[1] + minp[1])*Sy;

			glm::mat4 C = glm::mat4(
				Sx,0,0,0,
				0,Sy,0,0,
				0,0,1,0,
				0,0,0,1
			);
			//glm::mat4 Pz = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, minp[2], maxp[2]);
			glm::mat4 Pz = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
			//PMat[i] = C * Pz;
			PMat[i] = Pz;
		}

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rectangleTexture);
		glm::mat4 lightSpaceMatrix[4];
		for (int i = 0; i < 4; i++)
		{
			lightSpaceMatrix[i] = PMat[i] * lightView;
			ShadowMapShader->use();
			ShadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix[i]);
			ShadowMapShader->setVec3("lightPos", lightPos);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
			glClear(GL_DEPTH_BUFFER_BIT);
			renderScene(ShadowMapShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		
		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		SecondShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearp, farp);

		glm::mat4 view = camera.GetViewMatrix();
		SecondShader->setMat4("projection", projection);
		SecondShader->setMat4("view", view);
		// set light uniforms
		SecondShader->setVec3("viewPos", camera.Position);
		SecondShader->setVec3("lightPos", lightPos);
		SecondShader->setVec3("Front", camera.Front);
		SecondShader->setVec4("far_d", glm::vec4(zfar_i[1],zfar_i[2],zfar_i[3],zfar_i[4]));
		for (int i = 0; i < 4; i++)
		{
			ShadowMapShader->setMat4("lightSpaceMatrix["+to_string(i)+"]", lightSpaceMatrix[i]);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rectangleTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap[0]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap[1]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMap[2]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthMap[3]);
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


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, -1.5f, -8.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, -12.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -2.0f, 0.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.25));
		shader->setMat4("model", model);
		renderCube->drawObject();


	}
	void exit()
	{
		glDeleteFramebuffers(4, depthMapFBO);
		glDeleteTextures(1, &rectangleTexture);
		glDeleteTextures(4, depthMap);
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
		ImGui::Begin("CSM", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat("LightPoxX", &lightPos.x, -4.f, 4.f);
		ImGui::SliderFloat("LightPoxY", &lightPos.y, -4.f, 4.f);
		ImGui::SliderFloat("LightPoxZ", &lightPos.z, -4.f, 4.f);
		ImGui::End();
	}
};
#endif