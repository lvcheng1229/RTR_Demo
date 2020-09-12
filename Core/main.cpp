﻿
#include"TestDemo/TestDemo.h"
#include"ModelDemo/ModelDemo.h"

//Shadow
#include"ShadowMap.h"
#include"CSM.h"
#include"PCF.h"
#include"PCSS.h"
#include"VSM.h"
#include"ESM.h"
#include"ImprovedESM.h"
#include"ConvolutionSM.h"
#include"ShadowVolume.h"
#include"ProjectionShadow.h"

#include"UltSetting.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ListUI
{
public:
	TestDemo td;
	ModelDemo md;
	//Shadow
	ShadowMap shadowMap;
	CSM csm;
	PCF pcf;
	PCSS pcss;
	VSM vsm;
	ESM esm;
	ImprovedESM improvedesm;
	ConvolutionSM convolutionsm;
	ShadowVolume shadowvolume;
	ProjectionShadow projectionshadow;

	RenderLoop *rloop;
	ListUI()
	{
		rloop = &projectionshadow;
		rloop->changeDemo();
	}
	void DrawUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Text("%.1f FPS",ImGui::GetIO().Framerate);
		ImGui::Text("M:forbid mouse move\nN:allow mouse move");
		if (ImGui::CollapsingHeader("Test"))
		{
			ImGui::BeginChild("Child1", ImVec2(0, 80), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("TestDemo", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&td);
			ImGui::NextColumn();
			if(ImGui::Button("ModelDemo", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&md);
			ImGui::NextColumn();
			ImGui::EndChild();
		}
		if (ImGui::CollapsingHeader("Shadow(10Demos)"))
		{
			ImGui::BeginChild("Child2", ImVec2(0, 260), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("ShadowMap", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&shadowMap);
			if(ImGui::Button("CSM", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&csm);
			if(ImGui::Button("PCF", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pcf);
			if(ImGui::Button("PCSS", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pcss);
			if(ImGui::Button("VSM", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&vsm);
			if(ImGui::Button("ESM", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&esm);
			if(ImGui::Button("ImprovedESM", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&improvedesm);
			if(ImGui::Button("ConvolutionSM", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&convolutionsm);
			if(ImGui::Button("ShadowVolume", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&shadowvolume);
			if(ImGui::Button("ProjectionShadow", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&projectionshadow);
			ImGui::EndChild();
		}
		rloop->DrawUI();
		ImGui::Render();
		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void changeDemo(RenderLoop *test)
	{
		if (test->GetDemoName() == rloop->GetDemoName())
		{
			return;
		}			
		else
		{
			rloop->exit();
			rloop = test;
			rloop->changeDemo();
		}
		return;
	}
	RenderLoop *GetCurrentDemo()
	{
		return rloop;
	}
};
void InitUI()
{
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}
int main()
{

	if (!initWindow())
	{
		return -1;
	}

	InitUI();
	ListUI listui;
	RenderLoop *rLoop;
	while (!glfwWindowShouldClose(window))
	{
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		rLoop = listui.GetCurrentDemo();
		rLoop->update();
		listui.DrawUI();
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}