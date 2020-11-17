#ifndef CLUSTEREDFORWARD_H
#define CLUSTEREDFORWARD_H
#include<direct.h>
#include"RenderLoop.h"
#include"TextureBuffer.h"
#include <ctime>
const unsigned int NUM_LIGHTS=20;
class ClusteredForwardPlus:public RenderLoop
{
public:
	ClusteredForwardPlus() {
		initDemo();
	};

	Shader *commonShader;
	RenderedSphere *sphereObj;
	RenderedQuad *quadObj;

	GLuint woodTexture;
	glm::vec3 lightPos;

	Light lights[NUM_LIGHTS];

	TextureBuffer *_lightTexture;
	
	bool showNumlight=false;
	
	
	void initDemo()
	{
		SetDemoName("ClusteredForwardPlus");
		lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
	}

	void SettingBeforLoop()
	{		
		camera_params.Far = 100.0;
		camera_params.Near = 0.1;
		camera_params.Aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
		camera_params.Fov = camera.Zoom;

		/*lights[0].positions = glm::vec3(-6, 6, -2);
		lights[0].radius = 4;
		lights[0].color = glm::vec3(1, 0, 0);
			
		lights[1].positions = glm::vec3(-5, -5, -2);
		lights[1].radius = 10;
		lights[1].color = glm::vec3(0, 0.99, 0);

		lights[2].positions = glm::vec3(5, 5, -2);
		lights[2].radius = 4;
		lights[2].color = glm::vec3(0, 0, 0.99);

		lights[3].positions = glm::vec3(5, -5, -2);
		lights[3].radius = 10;
		lights[3].color = glm::vec3(0, 0.99, 0.99);*/
		
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator(time(0));

		float LIGHT_MIN[3] = { -12, -12, -12 };
		float LIGHT_MAX[3] = { 12, 12, 2 };

		for (int i = 0; i < NUM_LIGHTS; ++i)
		{
			lights[i].positions = glm::vec3(
				randomFloats(generator)*(LIGHT_MAX[0] - LIGHT_MIN[0]) + LIGHT_MIN[0],
				randomFloats(generator)*(LIGHT_MAX[1] - LIGHT_MIN[1]) + LIGHT_MIN[1],
				randomFloats(generator)*(LIGHT_MAX[2] - LIGHT_MIN[2]) + LIGHT_MIN[2]);
			lights[i].radius = 6+randomFloats(generator)*3;
			lights[i].color = glm::vec3(randomFloats(generator),
				randomFloats(generator),
				randomFloats(generator));
		}
		
		_lightTexture = new TextureBuffer(NUM_LIGHTS, 8);

		commonShader =new Shader("../../Core/ClusteredForwardPlus/clusteredForward.vert", "../../Core/ClusteredForwardPlus/clusteredForward.frag");

		woodTexture = loadTexture("../../Core/PublicResource/container.jpg");

		commonShader->use();
		commonShader->setInt("sampleTexture", 0);
		commonShader->setInt("u_lightbuffer", 1);
		commonShader->setInt("u_clusterbuffer", 2);

		sphereObj = new RenderedSphere();
		quadObj = new RenderedQuad();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);	

		InitClusteredRendered(15, 15, 15);
	}
	bool first = true;
	bool first1 = true;
	void update()
	{
		glm::mat4 view = camera.GetViewMatrix();
		
		camera_params.Fov = camera.Zoom;
		
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera_params.Fov), camera_params.Aspect, camera_params.Near, camera_params.Far);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (int i = 0; i < NUM_LIGHTS; i++)
		{
			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 0) + 0] = lights[i].positions.x;
			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 0) + 1] = lights[i].positions.y;
			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 0) + 2] = lights[i].positions.z;

			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 0) + 3] = lights[i].radius;

			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 1) + 0] = lights[i].color.x;
			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 1) + 1] = lights[i].color.y;
			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 1) + 2] = lights[i].color.z;

			_lightTexture->_buffer[_lightTexture->bufferIndex(i, 1) + 3] = 0;//dont store element
		}

		_lightTexture->update();
		updateClusters(view);
		
		commonShader->use();
		
		commonShader->setMat4("view", view);
		commonShader->setMat4("u_viewMatrix", view);
		commonShader->setMat4("projection", projection);

		commonShader->setInt("numLights", NUM_LIGHTS);

		commonShader->setVec3("u_cameraPos", camera.Position);

		commonShader->setFloat("cameraFOVScalar", fovScalar);
		commonShader->setFloat("cameraAspect", camera_params.Aspect);
		commonShader->setFloat("cameraNear", camera_params.Near);

		commonShader->setFloat("invRange", invRange);

		commonShader->setBool("useLogarithmic", USE_LOGARITHMIC);
		commonShader->setBool("useDynamic", USE_DYNAMIC);
		commonShader->setBool("showLightNum", showNumlight);

		commonShader->setInt("xSlices", xSlices);
		commonShader->setInt("ySlices", ySlices);
		commonShader->setInt("zSlices", zSlices);

		commonShader->setInt("textureHeight", floor((MAX_LIGHTS_PER_CLUSTER + 1) / 4 + 1));
		commonShader->setFloat("logOffset", LOG_OFFSET);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _lightTexture->_glTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _clusterTexture->_glTexture);

		renderScene(commonShader);
	}
	void renderScene(Shader *shader)
	{
		glm::mat4 model = glm::mat4(1.0f);

		for (int i = -6; i <= 6; i+=2)
		{
			for (int j = -6; j <= 6; j+=2)
			{
				for (int k = -8; k <= -2; k+=2)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(i, j, k));
					model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
					shader->setMat4("model", model);
					sphereObj->drawObject();
				}
			}
		}

	}
	void exit()
	{
		glDeleteTextures(1, &woodTexture);
		
		commonShader->exit();
		sphereObj->exit();
		quadObj->exit();

		_lightTexture->exit();
		_clusterTexture->exit();

		delete commonShader;
		delete sphereObj;
		delete quadObj;

		delete _lightTexture;
		delete _clusterTexture;
	}

	void DrawUI()
	{
		bool show_window = true;
		ImGui::Begin("TestDemo", &show_window, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("showLightNum", &showNumlight);
		ImGui::End();
	}

	TextureBuffer *_clusterTexture;

	int xSlices, ySlices, zSlices;

	const int MAX_LIGHTS_PER_CLUSTER = max(50, (int)NUM_LIGHTS / 3);
	const bool USE_DYNAMIC = true;
	const bool USE_LOGARITHMIC = false;
	const float LOG_OFFSET = USE_LOGARITHMIC ? 25.0 : 0.0;
	const float RANGE_SCALE = 0.02;

	//???
	float _farLight;
	float _nextFarLight;

	struct cameraParams
	{
		float Far, Near, Fov, Aspect;
	}camera_params;

	float invRange;
	float fovScalar;
	float min;
	void InitClusteredRendered(int _xSlices,int _ySlices, int _zSlices)
	{
		// Create a texture to store cluster data. Each row is a cluster, 
		//each column stores 4 light indices (1st column stores numlights and 3 indices
		_clusterTexture = new TextureBuffer(_xSlices*_ySlices*_zSlices, (MAX_LIGHTS_PER_CLUSTER + 1 / 4) + 1);
		xSlices = _xSlices;
		ySlices = _ySlices;
		zSlices = _zSlices;

		_farLight = 2000;
		_nextFarLight = 20;

		invRange = USE_LOGARITHMIC 
			? (1.0 / log(camera_params.Far - camera_params.Near + 1)) 
			: (1 / (camera_params.Far - camera_params.Near));

		fovScalar = tan(camera_params.Fov*(3.141592653589793284 / 360.0));
		min = camera_params.Near;
	}
	float scaleLogarithmically(float z) {
		if (z < 0) return -1;
		return log(z - min + 1) * invRange;
		//if use log invRange , z=far then return 1
	}
	float _max(float a, float b)
	{
		return a > b ? a : b;
	}
	float _min(float a, float b)
	{
		return a > b ? b : a;
	}
	void updateClusters(glm::mat4 viewMat)
	{

		_farLight = _nextFarLight;
		_nextFarLight = 0.0;

		//???
		float invDist = USE_DYNAMIC ? (camera_params.Far / _farLight) : (1 / RANGE_SCALE);
		
		commonShader->use();
		commonShader->setFloat("invDist", invDist);
		
		//clear light counts
		for (int x = 0; x < xSlices; ++x)
		{
			for (int y = 0; y < ySlices; ++y)
			{
				for (int z = 0; z < zSlices; ++z)
				{
					int i = x + y * xSlices + z * xSlices*ySlices;
					//note 1st column stores numlights and 3 indices
					_clusterTexture->_buffer[_clusterTexture->bufferIndex(i, 0)] = 0;
				}
			}
		}
		
		//for each light
		//determine which boxes it falls into
		//minimize false positives based on slides
		//count++ and add light index to each cluster

		for (int i = 0; i < NUM_LIGHTS; ++i)
		{
			glm::vec4 pos = glm::vec4(lights[i].positions,1.0);

			pos = viewMat * pos;

			pos[2] = -pos[2];//for sanity

			//Z-CLUSTERS
			//get the log-scaled z values of the sphere intercepts 
			//(I multiplied by 10 for this confined ??? scene)

			float closeZ = 0.0;
			float farZ = 0.0;

			if (USE_LOGARITHMIC) {
				closeZ = scaleLogarithmically(invDist * (pos[2] - lights[i].radius));
				farZ = scaleLogarithmically(invDist * (pos[2] + lights[i].radius));

			} else {
				closeZ = invDist * (pos[2] - lights[i].radius) * invRange;
				farZ = invDist * (pos[2] + lights[i].radius) * invRange;
			}
			if (farZ < 0) continue;

			closeZ = _max(0, _min(zSlices - 1, floor(closeZ*(zSlices + LOG_OFFSET) - LOG_OFFSET)));
			farZ = _max(0, _min(zSlices - 1, floor(farZ  *(zSlices + LOG_OFFSET) - LOG_OFFSET)));

			//Y-CLUSTERS
			float halfFrustumHeight = abs(pos[2]) * fovScalar;
			
			float invH = 1.0 / (2 * halfFrustumHeight);
			
			float topY = (pos[1] + lights[i].radius + halfFrustumHeight)*invH;
			float bottomY = (pos[1] - lights[i].radius + halfFrustumHeight) * invH;

			if (bottomY > 0.999 || topY < 0) continue;

			topY = _min(ySlices - 1, floor(topY*ySlices));
			bottomY = _max(0, floor(bottomY*ySlices));

			//X-CLUSTERS
			float halfFrustumWidth = camera_params.Aspect * halfFrustumHeight;
			
			float invW = 1 / (2 * halfFrustumWidth);
			float leftX = (pos[0] - lights[i].radius + halfFrustumWidth) * invW;
			float rightX = (pos[0] + lights[i].radius + halfFrustumWidth) * invW;
			if (leftX > 0.999 || rightX < 0) continue; 

			leftX = _max(0, floor(leftX * xSlices));
			rightX = _min(xSlices - 1, floor(rightX * xSlices));


			for (int x = leftX; x <= rightX; x++) {
				for (int y = bottomY; y <= topY; y++) {
					for (int z = closeZ; z <= farZ; z++) {
						int row = x + y * xSlices + z * xSlices * ySlices;
						int numLightsIndex = _clusterTexture->bufferIndex(row, 0);
						int currentLightCount = _clusterTexture->_buffer[numLightsIndex];

						if(currentLightCount < MAX_LIGHTS_PER_CLUSTER)
						{
							++currentLightCount;

							_clusterTexture->_buffer[numLightsIndex] = currentLightCount;

							int column = floor((currentLightCount) * 0.25);

							//int vec4Subscript = (currentLightCount) - column*4;
							int vec4Subscript=floor(currentLightCount % 4);
							
							int pixelIndex = _clusterTexture->bufferIndex(row, column);
							_clusterTexture->_buffer[pixelIndex + vec4Subscript] = i;
						}
					}
				}
			}
			if (pos[2] > _nextFarLight) _nextFarLight = pos[2];
		}
		_clusterTexture->update();
		first1 = false;
	}
};
#endif