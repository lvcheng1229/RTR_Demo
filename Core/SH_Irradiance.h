#ifndef SHIRRADIANCE_H
#define SHIRRADIANCE_H
#include<direct.h>
#include"RenderLoop.h"
#define SH_COUNT 9
class SH_Irradiance:public RenderLoop
{
public:
	SH_Irradiance() {
		initDemo();
	};

	typedef struct
	{
		double coeffs[SH_COUNT];
	} ShChannel;
	const double M_PI = 3.141592653589793284;

	Shader *commonShader;
	Shader *backgroundShader;

	RenderedSphere *sphereObj;
	RenderedCube *renderCube;

	GLuint woodTexture;
	GLuint cubemapTexture;
	glm::vec3 lightPos;

	std::vector<std::vector<short int>>datas;
	int width, height, nrChannels;
	ShChannel *shChannel;

	bool useCubemap = false;
	enum 
	{
		CUBE_FACE_RIGHT = 0, // x+
		CUBE_FACE_LEFT,  // x-
		CUBE_FACE_TOP,   // y+
		CUBE_FACE_BOTTOM,// y-
		CUBE_FACE_BACK,  // z+
		CUBE_FACE_FRONT, // z-
		CUBE_FACE_COUNT
	} CubeFace;





	void initDemo()
	{
		SetDemoName("SH_Irradiance");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}

	void SettingBeforLoop()
	{		
		commonShader =new Shader("../../Core/SH_Irradiance/lightShader.vert", "../../Core/SH_Irradiance/lightShader.frag");
		backgroundShader =new Shader("../../Core/SH_Irradiance/background.vert", "../../Core/SH_Irradiance/background.frag");

		woodTexture = loadTexture("../../Core/PublicResource/container.jpg");
		
		vector<std::string> faces
		{
			"../../Core/SH_Irradiance/right.png",
			"../../Core/SH_Irradiance/left.png",
			"../../Core/SH_Irradiance/top.png",
			"../../Core/SH_Irradiance/bottom.png",
			"../../Core/SH_Irradiance/front.png",
			"../../Core/SH_Irradiance/back.png"
		};
		datas = loadCubeTexture(faces, &cubemapTexture, &width, &height, &nrChannels);

		

		backgroundShader->use();
		backgroundShader->setInt("environmentMap", 0);

		sphereObj = new RenderedSphere();
		renderCube = new RenderedCube();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		shChannel = new ShChannel[nrChannels];
			
		sh_integrate_cubemap(datas,
			width,
			height,
			nrChannels,
			shChannel);

		commonShader->use();
		commonShader->setInt("environmentMap", 0);
		for (int i = 0; i < 9; i++)commonShader->setFloat("sh_coef_r[" + to_string(i) + "]", shChannel[0].coeffs[i]);
		for (int i = 0; i < 9; i++)commonShader->setFloat("sh_coef_g[" + to_string(i) + "]", shChannel[1].coeffs[i]);
		for (int i = 0; i < 9; i++)commonShader->setFloat("sh_coef_b[" + to_string(i) + "]", shChannel[2].coeffs[i]);

	}

	void update()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		commonShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		commonShader->setMat4("view", view);
		commonShader->setMat4("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		commonShader->setMat4("model", model);
		commonShader->setBool("useCubemap", useCubemap);
		sphereObj->drawObject();

		backgroundShader->use();
		backgroundShader->setMat4("view", view);
		backgroundShader->setMat4("projection", projection);
		// skybox cube
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		renderCube->drawObject();
	}

	void exit()
	{
		glDeleteTextures(1, &woodTexture);
		glDeleteTextures(1, &cubemapTexture);

		commonShader->exit();
		backgroundShader->exit();

		sphereObj->exit();
		renderCube->exit();

		delete commonShader;
		delete backgroundShader;

		delete sphereObj;
		delete renderCube;

		delete shChannel;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("SH_Irradiance", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("useCubemap", &useCubemap);
		ImGui::End();
	}
	

	double sh_eval_9(int i, double x, double y, double z)
	{
		switch (i)
		{
		case 0:
			return 0.5 * sqrt(1.0 / M_PI);
		case 1:
			return x * 0.5 * sqrt(3.0 / M_PI);
		case 2:
			return z * 0.5 * sqrt(3.0 / M_PI);
		case 3:
			return y * 0.5 * sqrt(3.0 / M_PI);
		case 4:
			return x * z * 0.5 * sqrt(15.0 / M_PI);
		case 5:
			return y * z * 0.5 * sqrt(15.0 / M_PI);
		case 6:
			return x * y * 0.5 * sqrt(15.0 / M_PI);
		case 7:
			return (3.0 * z*z - 1.0) * 0.25 * sqrt(5.0 / M_PI);
		case 8:
			return (x*x - y*y) * 0.25 * sqrt(15.0 / M_PI);
		default:
			assert("sh_eval9");
			return 0;
		}
	}

	static inline void uv_to_cube(double u, double v, int face, glm::vec3 *out_dir)
	{
		switch (face)
		{
		case CUBE_FACE_RIGHT:
			(*out_dir)[0] = 1.0f;
			(*out_dir)[1] = v;
			(*out_dir)[2] = -u;
			break;
		case CUBE_FACE_LEFT:
			(*out_dir)[0] = -1.0f;
			(*out_dir)[1] = v;
			(*out_dir)[2] = u;
			break;
		case CUBE_FACE_TOP:
			(*out_dir)[0] = u;
			(*out_dir)[1] = 1.0f;
			(*out_dir)[2] = -v; 
			break;
		case CUBE_FACE_BOTTOM:
			(*out_dir)[0] = u;
			(*out_dir)[1] = -1.0f;
			(*out_dir)[2] = v;
			break;
		case CUBE_FACE_BACK: 
			(*out_dir)[0] = u;
			(*out_dir)[1] = v;
			(*out_dir)[2] = 1.0f;
			break;
		case CUBE_FACE_FRONT:
			(*out_dir)[0] = -u;
			(*out_dir)[1] = v;
			(*out_dir)[2] = -1.0f;
			break;
		}
	}


	static double surface_area(double x, double y)
	{
		return atan2(x * y, sqrt(x * x + y * y + 1.0));
	}



	void sh_integrate_cubemap(const std::vector<std::vector<short int>> face_data,
		unsigned int width,
		unsigned int height,
		unsigned int components_per_pixel,
		ShChannel* out_channels)
	{
		// zero out coeffecients for accumulation
		for (int comp = 0; comp < components_per_pixel; ++comp)
		{
			for (int s = 0; s < SH_COUNT; ++s)
				out_channels[comp].coeffs[s] = 0.0;
		}

		for (int face = 0; face < CUBE_FACE_COUNT; ++face)
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					// center each pixel
					double px = (double)x + 0.5;
					double py = (double)y + 0.5;
					// normalize into [-1, 1] range
					double u = 2.0 * (px / (double)width) - 1.0;
					double v = 2.0 * (py / (double)height) - 1.0;

					// calculate the solid angle
					double d_x = 1.0 / (double)width;
					double x0 = u - d_x;
					double y0 = v - d_x;
					double x1 = u + d_x;
					double y1 = v + d_x;
					double d_a = surface_area(x0, y0) - surface_area(x0, y1) - surface_area(x1, y0) + surface_area(x1, y1);

					// find vector on unit sphere
					glm::vec3 dir;
					uv_to_cube(u, v, face, &dir);
					dir = glm::normalize(dir);

					size_t pixel_start = (x + y * width) * components_per_pixel;

					for (int s = 0; s < SH_COUNT; ++s)
					{
						double sh_val = sh_eval_9(s, dir[0], dir[1], dir[2]);

						for (int comp = 0; comp < components_per_pixel; ++comp)
						{
							double col = face_data[face][pixel_start + comp] / 255.0;
							out_channels[comp].coeffs[s] += col * sh_val * d_a;
						}
					}
				}
			}
		}
	}
};
#endif