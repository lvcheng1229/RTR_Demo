
#include"TestDemo/TestDemo.h"
#include"ModelDemo/ModelDemo.h"
#include"UltSetting.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ListUI
{
public:
	TestDemo td;
	ModelDemo md;
	RenderLoop *rloop;
	ListUI()
	{
		rloop = &td;
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
			ImGui::BeginChild("ChildR", ImVec2(0, 80), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("TestDemo", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&td);
			ImGui::NextColumn();
			if(ImGui::Button("ModelDemo", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&md);
			ImGui::NextColumn();
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