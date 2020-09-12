#ifndef SHADOWVOLUME_H
#define SHADOWVOLUME_H
#include<direct.h>
#include"RenderLoop.h"
class ShadowVolume:public RenderLoop
{
public:
	ShadowVolume() {
		initDemo();
	};

	Shader *commonShader;
	Shader *VolumeShader;
	Shader *shadowShader;

	RenderedSphere *sphereObj;
	RenderedQuad *renderQuad;
	RenderedTriangle *renderTriangle;

	GLuint woodTexture;
	glm::vec3 lightPos;

	float volumeLength = 30.0f;
	
	GLuint triangleVolumeVAO=0;
	GLuint triangleVolumeVBO=0;
	
	bool showShadowVolume = false;
	bool canRotate = true;

	float TrianglePos[3];
	float LPos[3];
	void initDemo()
	{
		SetDemoName("ShadowVolume");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
		
		TrianglePos[0] = 2;
		TrianglePos[1] = 1;
		TrianglePos[2] = 0;

		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];
		}

		showShadowVolume = false;
		canRotate = true;
	}

	
	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/PublicResource/commonshader.vs", "../../Core/PublicResource/commonshader.fs");
		VolumeShader =new Shader("../../Core/ShadowVolume/emptyShader.vert", "../../Core/ShadowVolume/emptyShader.frag");
		shadowShader = new Shader("../../Core/ShadowVolume/emptyShader.vert", "../../Core/ShadowVolume/emptyShader.frag");

		woodTexture = loadTexture("../../Core/PublicResource/rectangle.jpg");

		commonShader->use();
		commonShader->setInt("sampleTexture", 0);

		sphereObj = new RenderedSphere();
		renderQuad = new RenderedQuad();
		renderTriangle = new RenderedTriangle();
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable( GL_STENCIL_TEST );
	}

	void update()
	{
		//Step 1 正常绘制场景
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		VolumeShader->use();
		VolumeShader->setMat4("view", view);
		VolumeShader->setMat4("projection", projection);

		commonShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		commonShader->setMat4("view", view);
		commonShader->setMat4("projection", projection);
		commonShader->setVec3("lightcolor", glm::vec3(1.0f, 1.0f, 1.0f));
		commonShader->setVec3("lightPos", lightPos);
		commonShader->setVec3("viewPos", camera.Position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -2.0f));
		commonShader->setMat4("model", model);
		sphereObj->drawObject();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0));
		commonShader->setMat4("model", model);
		renderQuad->drawObject();

		model = glm::mat4(1.0f);
		if(canRotate)
			model = glm::rotate(model, glm::radians((float)glfwGetTime()*60), glm::vec3(0.0,1.0,0.0));
				
		model = glm::translate(model, glm::vec3(TrianglePos[0], TrianglePos[1], TrianglePos[2]));	
		commonShader->setMat4("model", model);
		renderTriangle->drawObject();

		VolumeShader->use();
		VolumeShader->setMat4("model",glm::mat4(1.0f));
		glm::mat4 triModel = model;
		if (showShadowVolume)
		{
			buildTriangleVolume(triModel);
		}
		
		//Step 2 写入模板缓冲
		VolumeShader->use();
		glStencilMask(0xFF);
		glDepthMask( GL_FALSE );
		glDepthFunc(GL_LEQUAL );
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glStencilFunc( GL_ALWAYS, 1, 0xFF );

		glEnable(GL_CULL_FACE);

		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		VolumeShader->setMat4("model",glm::mat4(1.0f));
		buildTriangleVolume(triModel);

		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		VolumeShader->setMat4("model",glm::mat4(1.0f));
		buildTriangleVolume(triModel);


		//Step 3 绘制一个屏幕大小的黑色矩形 只在那些模板缓冲不为0的地方绘制阴影
		glStencilMask(0x00);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glDisable(GL_CULL_FACE);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		shadowShader->use();
		shadowShader->setMat4("view", glm::mat4(1.0f));
		shadowShader->setMat4("projection", glm::mat4(1.0f));
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(10.0, 10.0, 1.0));
		shadowShader->setMat4("model", model);
		renderQuad->drawObject();
	
		glDepthMask( GL_TRUE );
		glStencilFunc( GL_ALWAYS, 1, 0xFF);

	}

	void exit()
	{
		glDisable(GL_STENCIL_TEST);

		glDeleteTextures(1, &woodTexture);
		glDeleteBuffers(1, &triangleVolumeVBO);
		glDeleteVertexArrays(1, &triangleVolumeVAO);

		commonShader->exit();
		VolumeShader->exit();
		shadowShader->exit();

		sphereObj->exit();
		renderQuad->exit();
		renderTriangle->exit();

		delete commonShader;
		delete VolumeShader;
		delete shadowShader;

		delete renderQuad;
		delete sphereObj;
		delete renderTriangle;
	}
	void DrawUI()
	{
		
		bool show_window = true;
		ImGui::Begin("ShadowVolume(ZPass)", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("Show Shadow Volume", &showShadowVolume);
		ImGui::Checkbox("CanRotate", &canRotate);

		ImGui::SliderFloat3("TrianglePos", TrianglePos, -2.0, 2.0);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);

		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}

	
	glm::vec3 computePos(const glm::vec3 &pos)
	{
		return pos + glm::normalize((pos - lightPos))*volumeLength;
	}

	vector<glm::vec3> buildVolumeForEdge(const glm::vec3 &posA,const glm::vec3 &posB)
	{
		vector<glm::vec3>data;

		glm::vec3 posAA = computePos(posA);
		glm::vec3 posBB = computePos(posB);

		//注意输入的两点应为逆时针排列
		data.push_back(posA);
		data.push_back(posAA);
		data.push_back(posB);

		data.push_back(posAA);
		data.push_back(posBB);
		data.push_back(posB);

		return data;
	}

	void buildTriangleVolume(glm::mat4 model)
	{
		vector<float>vertics;
		vector<glm::vec3>data;
		glm::vec3 pos1(0.0f, 0.0f, 0.0f); pos1 = glm::vec3(model*glm::vec4(pos1, 1.0));
		glm::vec3 pos2(1.0f, 0.0f, -1.0f);pos2 = glm::vec3(model*glm::vec4(pos2, 1.0));
		glm::vec3 pos3(0.0f, 0.0f, -1.0f);pos3 = glm::vec3(model*glm::vec4(pos3, 1.0));

		vector<glm::vec3>temp;
		temp = buildVolumeForEdge(pos1, pos2);
		for (auto t : temp)
			data.push_back(t);

		temp = buildVolumeForEdge(pos2, pos3);
		for (auto t : temp)
			data.push_back(t);

		temp = buildVolumeForEdge(pos3, pos1);
		for (auto t : temp)
			data.push_back(t);

		for (auto t : data)
		{
			vertics.push_back(t.x);
			vertics.push_back(t.y);
			vertics.push_back(t.z);
		}

		if (triangleVolumeVBO == 0)
		{		
			glGenBuffers(1, &triangleVolumeVBO);
			glBindBuffer(GL_ARRAY_BUFFER, triangleVolumeVBO);
			glBufferData(GL_ARRAY_BUFFER, vertics.size() * sizeof(float), &vertics[0], GL_STREAM_DRAW);
			glGenVertexArrays(1, &triangleVolumeVAO);

			glBindVertexArray(triangleVolumeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
			glBindVertexArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, triangleVolumeVBO);
		glBufferData(GL_ARRAY_BUFFER, vertics.size() * sizeof(float), &vertics[0], GL_STATIC_DRAW);


		glBindVertexArray(triangleVolumeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);
	}
};
#endif