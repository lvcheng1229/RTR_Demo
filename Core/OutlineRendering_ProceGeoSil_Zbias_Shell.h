
#ifndef ORPROCEGEOSIL_H
#define ORPROCEGEOSIL_H
#include<direct.h>
#include"RenderLoop.h"
class OutlineRendering_ProceGeoSil:public RenderLoop
{
public:
	OutlineRendering_ProceGeoSil() {
		initDemo();
	};
	Shader *BackfaceShader;
	Shader *FrontFaceShader;

	RenderedCube *renderCube;
	RenderedSphere *renderSphere;

	Model *bunny;

	GLuint containerTexture;
	glm::vec3 lightPos;

	float LPos[3];
	float EdgeCol[3];
	float edgeWidth = 0.02;
	bool zbiasOrShell = true;
	void initDemo()
	{
		SetDemoName("OR_ProceGeoSil_Zbias_Shell");
		lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

		edgeWidth = 0.02;
		for (int i = 0; i < 3; i++)
		{
			LPos[i] = lightPos[i];
			
		}
		EdgeCol[0] = 0.8235;EdgeCol[1] = 0.47;EdgeCol[2] = 0.47;
	}
	//-----------------------------------------------
	//1.向前移动后向面(zBias 2.在三角形所在的平面内拓展三角形的三个顶点(flattenTriangle
	//2.移动三角面片的顶点(shell Method
	//https://zhuanlan.zhihu.com/p/84075550
	//
	//          shell method
	//如果我们只是简单的进行扩展的话，有可能导致背面的面片沿法线进行扩展后的Z值比原先需要渲染的面片的Z值小
	//盖住前面需要渲染的面片，导致模型渲染出错，这种情况在凹模型容易出现.
	// 我们需要对变换后的法线进行一个扁平化处理normal.z = -0.4;，让第一个Pass得到的模型轮廓成为一个平面
	//-----------------------------------------------
	void SettingBeforLoop()
	{			
		containerTexture = loadTexture("../../Core/PublicResource/container.jpg");
		BackfaceShader = new Shader("../../Core/OutlineRendering/zbiasShellProceGeoSil.vert", "../../Core/OutlineRendering/zbiasShellProceGeoSil.frag");
		FrontFaceShader = new Shader("../../Core/PublicResource/noTexShader.vert", "../../Core/PublicResource/noTexShader.frag");
		bunny = new Model("../../Core/PublicResource/bunny.obj");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		renderCube = new RenderedCube();
		renderSphere = new RenderedSphere();
	}

	void update()
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(91.0 / 255.0, 194.0 / 255.0, 231.0 / 255.0, 1.0);
	
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glEnable(GL_CULL_FACE);

		glCullFace(GL_FRONT);
		BackfaceShader->use();
		BackfaceShader->setMat4("projection", projection);
		BackfaceShader->setMat4("view", view);

		BackfaceShader->setVec3("edgeColor", glm::vec3(EdgeCol[0], EdgeCol[1], EdgeCol[2]));
		BackfaceShader->setFloat("edgeWidth", edgeWidth);
		BackfaceShader->setBool("zbaisOrShell", zbiasOrShell);
		renderScene(BackfaceShader);

		glCullFace(GL_BACK);
		FrontFaceShader->use();
		FrontFaceShader->setMat4("projection", projection);
		FrontFaceShader->setMat4("view", view);
		FrontFaceShader->setVec3("viewPos", camera.Position);
		FrontFaceShader->setVec3("lightPos", lightPos);
		FrontFaceShader->setVec3("lightcolor", glm::vec3(1, 1, 1));
		FrontFaceShader->setVec3("objColor", glm::vec3(0.8, 0.8, 0.2));
		renderScene(FrontFaceShader);
	}
	void renderScene(Shader *shader)
	{		
		glm::mat4 model = glm::mat4(1.0f);

		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube->drawObject();

		//sphere
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderSphere->drawObject();

		//bunny
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -1.3f, -3.0f));
		model = glm::scale(model, glm::vec3(8.0f));
		shader->setMat4("model", model);
		bunny->Draw(*shader);
	}
	void exit()
	{
		glDeleteTextures(1, &containerTexture);

		renderCube->exit();
		renderSphere->exit();
		BackfaceShader->exit();
		bunny->exit();
		FrontFaceShader->exit();

		delete renderCube;
		delete renderSphere;
		delete BackfaceShader;
		delete bunny;
		delete FrontFaceShader;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("OR_ProceGeoSil_Zbias_Shell", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::SliderFloat3("LightPos", LPos, -5.0, 5.0);
		ImGui::SliderFloat("EdgeWidth", &edgeWidth, 0.0, 0.8);
		ImGui::ColorEdit3("EdgeColor", EdgeCol);
		ImGui::Checkbox("Zbias(T)/Shell(F)", &zbiasOrShell);
		for (int i = 0; i < 3; i++)
		{
			lightPos[i] = LPos[i];
		}

		ImGui::End();
	}
};
#endif