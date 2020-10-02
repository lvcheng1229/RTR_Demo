#ifndef PBR_H
#define PBR_H
#include<direct.h>
#include"RenderLoop.h"
class PBR:public RenderLoop
{
public:
	PBR() {
		initDemo();
	};

	Shader *shader;
	RenderedSphere *sphereObj;

	//unsigned int albedo;
	//unsigned int normal;
	//unsigned int metallic;
	//unsigned int roughness;
	//unsigned int ao;

	glm::vec3 lightPositions[4] = {
		glm::vec3(-10.0f,  7.0f, -10.0f),
		glm::vec3( 10.0f,  7.0f, -10.0f),
		glm::vec3(-10.0f, -13.0f, -10.0f),
		glm::vec3( 10.0f, -13.0f, -10.0f),
	};
	glm::vec3 lightColors[4] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;
	void initDemo()				 
	{
		SetDemoName("PBR");
			
	}

	void SettingBeforLoop()
	{		
		shader=new Shader("../../Core/PBR/pbr.vert", "../../Core/PBR/pbr.frag");

		shader->use();
		shader->setVec3("albedo", 0.5f, 0.0f, 0.0f);
		shader->setFloat("ao", 1.0f);

		sphereObj = new RenderedSphere();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

	}

	void update()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader->setMat4("projection", projection);
		
		glm::mat4 view = camera.GetViewMatrix();
		shader->setMat4("view", view);
		shader->setVec3("camPos", camera.Position);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, albedo);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, normal);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, metallic);
		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, roughness);
		//glActiveTexture(GL_TEXTURE4);
		//glBindTexture(GL_TEXTURE_2D, ao);

		
		glm::mat4 model = glm::mat4(1.0f);
		for (int row = 0; row < nrRows; ++row) 
		{
			shader->setFloat("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col) 
			{
				// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				shader->setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.025f, 1.0f));

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(col - (nrColumns / 2)) * spacing, 
					(row - (nrRows / 2)) * spacing, 
					-20.0f
				));
				shader->setMat4("model", model);
				sphereObj->drawObject();
			}
		}

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			//newPos = lightPositions[i];
			shader->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			shader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			shader->setMat4("model", model);
			sphereObj->drawObject();
		}
	}

	void exit()
	{
		//glDeleteTextures(1, &albedo);
		//glDeleteTextures(1, &normal);
		//glDeleteTextures(1, &metallic);
		//glDeleteTextures(1, &roughness);
		//glDeleteTextures(1, &ao);


		shader->exit();
		sphereObj->exit();

		delete shader;
		delete sphereObj;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("PBR", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Text("This is TestDemo");
		ImGui::End();
	}
};
#endif