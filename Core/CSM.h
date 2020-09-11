
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
	float nearp = 1, farp = 100.0;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	void initDemo()
	{
		SetDemoName("CSM");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		
		nearp = 1; 
		farp = 100.0;
	}

	void SettingBeforLoop()
	{			
		//����near��farȷ����������׶���near��far
		for (int i = 0; i <= N; i++)
		{
			zfar_i[i] = nearp * pow(farp / nearp, (float)i / (float)N);
		}

		ShadowMapShader =new Shader("../../Core/CSM/ShadowMapShader.vert", "../../Core/CSM/ShadowMapShader.frag");
		SecondShader =new Shader("../../Core/CSM/secondShader.vert", "../../Core/CSM/secondShader.frag");
		rectangleTexture = loadTexture("../../Core/PublicResource/rectangle.jpg");

		glGenTextures(4, depthMap);
		for (int i = 0; i < 4; i++)
		{
			glBindTexture(GL_TEXTURE_2D, depthMap[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//��ѯ��Ӱͼʱ��������0,1����Χ�ĵ���Ⱦ���Ϊ1����������Ӱ
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

		//������׶��
		glm::vec3 frustumPos[5][4];		
		for (int i = 0; i <= N; i++)
		{
			float tempD = zfar_i[i];
			float hd2 = tempD * tan(camera.Zoom / 2.0*3.1415926535 / 180.0);

			//�������������׵����ĸ�������worldSpace�е����ꡣ
			glm::vec3 wd2vec = glm::normalize(camera.Right)*hd2*(float)SCR_WIDTH / (float)SCR_HEIGHT;
			glm::vec3 hd2vec = glm::normalize(camera.Up)*hd2;
			glm::vec3 d = camera.Position + glm::normalize(camera.Front)*tempD;

			frustumPos[i][0] = d + wd2vec + hd2vec;
			frustumPos[i][1] = d + wd2vec - hd2vec;
			frustumPos[i][2] = d - wd2vec + hd2vec;
			frustumPos[i][3] = d - wd2vec - hd2vec;
		}

		glm::vec3 center = glm::vec3(0.1f);
		glm::vec3 lightUp = glm::vec3(0.0, 1.0, 0.0);

		glm::mat4 lighdextView = glm::lookAt(lightPos,center ,lightUp );

		//������Դ�ֲ�����ϵ
		glm::vec3 lightDir = glm::normalize(center - lightPos);
		glm::vec3 lightRight = glm::normalize(glm::cross(lightDir, lightUp));
		lightUp = glm::normalize(glm::cross(lightRight, lightDir));
		
		for (int i = 0; i <= N; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				//���������׵��任��lightSpace��
				glm::vec3 tempVec;
				tempVec.x = glm::dot(frustumPos[i][j]-lightPos,lightRight);
				tempVec.y = glm::dot(frustumPos[i][j]-lightPos,lightUp);
				tempVec.z = glm::dot(frustumPos[i][j]-lightPos,lightDir);
				frustumPos[i][j] = tempVec;
			}
		}

		
		glm::mat4 PMat[4];
		glm::mat4 lightSpaceMatrix[4];
		for (int i = 0; i < N; i++)
		{
			glm::vec3 tempVec;
			glm::vec3 minp(1000, 1000, 1000);
			glm::vec3 maxp(-1000, -1000, -1000);
			//���������׶���ڹ�Դ�ռ��е����ֵ����Сֵ
			for (int j = 0; j < 4; j++)
			{
				for (int l = 0; l <= 1; l++)
				{
					tempVec = frustumPos[i+l][j];
					for (int k = 0; k < 3; k++)
					{
						if (tempVec[k] < minp[k])minp[k] = tempVec[k];
						if (tempVec[k] > maxp[k])maxp[k] = tempVec[k];
					}
				}
			}
			//�������ֵ����Сֵ�ı�ƽ�й����׶�壬��ʹ��Դ��׶����ܰ����������׵��
			float Wx = (maxp[0] - minp[0]) / 2.0;
			float Wy = (maxp[1] - minp[1]) / 2.0;
			float Ox = 0.5*(maxp[0] + minp[0]);
			float Oy = 0.5*(maxp[1] + minp[1]);
			glm::vec3 newLightpos = lightPos + lightRight * Ox + lightUp * Oy;
			
			//����׶��Ŵ󣬱���������ڲ�ͬ���ͼ֮����Ծʱ��������������
			glm::mat4 P = glm::ortho(-Wx*1.4f, Wx*1.4f, -Wy*1.4f, Wy*1.4f,0.1f, 30.0f);

			glm::mat4 newLightView = glm::lookAt(lightPos, lightPos + lightDir, lightUp);
			lightSpaceMatrix[i] = P * newLightView;
		}

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rectangleTexture);
		
		for (int i = 0; i < 4; i++)
		{
			ShadowMapShader->use();
			ShadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix[i]);
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
		
		SecondShader->setVec3("viewPos", camera.Position);
		SecondShader->setVec3("lightPos", lightPos);
		SecondShader->setVec3("Front", camera.Front);

		//�����������׶���farPlane���뵽far_d���Ա���shader�н��в���
		glm::vec4 far_d = glm::vec4(zfar_i[1], zfar_i[2], zfar_i[3], zfar_i[4]);		
		SecondShader->setVec4("far_d", far_d);	

		for (int i = 0; i < 4; i++)
		{
			SecondShader->setMat4("lightSpaceMatrix["+to_string(i)+"]", lightSpaceMatrix[i]);
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