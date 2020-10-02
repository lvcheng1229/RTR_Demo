#ifndef PBRTEX_H
#define PBRTEX_H
#include<direct.h>
#include"RenderLoop.h"
class PBR_TEX:public RenderLoop
{
public:
	PBR_TEX() {
		initDemo();
	};

	Shader *shader;
	RenderedSphere *sphereObj;

	unsigned int albedo;
	unsigned int normal;
	unsigned int metallic;
	unsigned int roughness;
	unsigned int ao;

	glm::vec3 lightPositions[4] = {
		glm::vec3(-10.0f,  7.0f, 0.0f),
		glm::vec3( 10.0f,  7.0f, 0.0f),
		glm::vec3(-10.0f, -13.0f, 0.0f),
		glm::vec3( 10.0f, -13.0f, 0.0f),
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
		SetDemoName("PBR_TEX");

	}

	void SettingBeforLoop()
	{		
		shader=new Shader("../../Core/PBR/pbr.vert", "../../Core/PBR/pbrTex.frag");

		shader->use();
		shader->setInt("albedoMap", 0);
		shader->setInt("normalMap", 1);
		shader->setInt("metallicMap", 2);
		shader->setInt("roughnessMap", 3);
		shader->setInt("aoMap", 4);

		// load PBR material textures
		// --------------------------
		albedo    = loadTextureMipMap("../../Core/PBR/basecolor.png");
		normal    = loadTextureMipMap("../../Core/PBR/normal.png");
		metallic  = loadTextureMipMap("../../Core/PBR/metallic.png");
		roughness = loadTextureMipMap("../../Core/PBR/roughness.png");
		ao        = loadTextureMipMap("../../Core/PBR/ao.png");

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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao);
		// render rows*column number of spheres with material properties defined by textures (they all have the same material properties)
		glm::mat4 model = glm::mat4(1.0f);
		for (int row = 0; row < nrRows; ++row)
		{
			for (int col = 0; col < nrColumns; ++col)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(float)(col - (nrColumns / 2)) * spacing,
					(float)(row - (nrRows / 2)) * spacing,
					-10.0f
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
			newPos = lightPositions[i];
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
		glDeleteTextures(1, &albedo);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &metallic);
		glDeleteTextures(1, &roughness);
		glDeleteTextures(1, &ao);


		shader->exit();
		sphereObj->exit();

		delete shader;
		delete sphereObj;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("PBR_TEX", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Text("This is TestDemo");
		ImGui::End();
	}
};
#endif