
#include"TestDemo/TestDemo.h"
#include"ModelDemo/ModelDemo.h"
#include"NPRTest.h"

//Shadow 10
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

//NPR 9
#include"OutlineRendering_NormalEdge.h"
#include"OutlineRendering_ProceGeoSil_Zbias_Shell.h"
#include"OutlineRendering_ProceGeoSil_FattenTriangle.h"
#include"OutlineRendering_ImageBased.h"
#include"XToon_HighLight.h"
#include"XToon_BackLight.h"
#include"XToon_AerialPerspective.h"
#include"Lines.h"
#include"Hatch.h"

//Image Space Effect 5
#include"HDR.h"
#include"Bloom_Blur.h"
#include"Bloom_DownSample.h"
#include"DOF_Metod1.h"
#include"MotionBlur.h"

//PBR 5
#include"PBR.h"
#include"PBR_TEX.h"
#include"PBR_NDF.h"
#include"PBR_G.h"
#include"PBR_Diffuse.h"

//Local and Global Ilumination
#include"IBL_Diffuse.h"
#include"IBL_Specular.h"
#include"SSAO.h"
#include"SSAOHB.h"
#include"LTC.h"
#include"SH_Irradiance.h"

#include"UltSetting.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


class ListUI
{
public:
	//Test
	TestDemo td;
	ModelDemo md;
	NPRTEST nprtest;

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

	//NPR
	OutlineRendering_NormalEdge or_normaledge;
	OutlineRendering_ProceGeoSil or_procegeosil_zbias_shell;
	OutlineRendering_ProceGeoSil_FattenTriangle or_procegeosil_fattentriangle;
	OutlineRendering_ImageBased or_imagebased;
	XToon_HighLight xtoon_highlight;
	XToon_BackLight xtoon_backlight;
	XToon_AerialPerspective xtoon_aerialperspective;
	NPRLines nprlines;
	Hatch hatch;

	//Image Space Effect
	Bloom_Blur bloom_blur;
	Bloom_DownSample bloom_downsample;
	DOF_Metod1 dof_metod1;
	MotionBlur motionblur;

	//PBR
	PBR pbr;
	PBR_TEX pbr_tex;
	PBR_NDF pbr_ndf;
	PBR_G pbr_g;
	PBR_Diffuse pbr_diffuse;

	//Local Ilumination
	IBL_Diffuse ibl_diffuse;
	IBL_Specular ibl_specular;
	SSAO ssao;
	SSAOHB ssaohb;
	LTC ltc;
	SH_Irradiance sh_irradiance;

	RenderLoop *rloop;
	ListUI()
	{
		rloop = &sh_irradiance;
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
			if(ImGui::Button("ModelDemo", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&md);
			if(ImGui::Button("NPRTest", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&nprtest);
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
		if (ImGui::CollapsingHeader("NPR(9Demos)"))
		{
			ImGui::BeginChild("Child2", ImVec2(0, 240), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("OR_NormalEdge", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&or_normaledge);
			if(ImGui::Button("OR_ProceGeoSil_Zbias_Shell", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&or_procegeosil_zbias_shell);
			if(ImGui::Button("OR_ProceGeoSil_FattenTriangle", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&or_procegeosil_fattentriangle);
			if(ImGui::Button("OR_ImageBased", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&or_imagebased);
			if(ImGui::Button("XToon_HighLight", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&xtoon_highlight);
			if(ImGui::Button("XToon_BackLight", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&xtoon_backlight);
			if(ImGui::Button("XToon_AerialPerspective", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&xtoon_aerialperspective);
			if(ImGui::Button("NPRLines", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&nprlines);
			if(ImGui::Button("Hatch", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&hatch);
			ImGui::EndChild();
		}
		if (ImGui::CollapsingHeader("ImageSpaceEffect"))
		{
			ImGui::BeginChild("Child2", ImVec2(0, 240), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("Bloom_Blur", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&bloom_blur);
			if(ImGui::Button("Bloom_DownSample", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&bloom_downsample);
			if(ImGui::Button("DOF_Metdod1", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&dof_metod1);
			if(ImGui::Button("MotionBlur", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&motionblur);
			ImGui::EndChild();
		}
		if (ImGui::CollapsingHeader("PBR"))
		{
			ImGui::BeginChild("Child2", ImVec2(0, 160), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("PBR", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pbr);
			if(ImGui::Button("PBR_TEX", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pbr_tex);
			if(ImGui::Button("PBR_NDF", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pbr_ndf);
			if(ImGui::Button("PBR_G", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pbr_g);
			if(ImGui::Button("PBR_Diffuse", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&pbr_diffuse);
			ImGui::EndChild();
		}
		if (ImGui::CollapsingHeader("LocalIllumination"))
		{
			ImGui::BeginChild("Child2", ImVec2(0, 240), true, ImGuiWindowFlags_MenuBar);
			ImGui::Columns(1);
			if(ImGui::Button("IBL_Diffuse", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&ibl_diffuse);
			if(ImGui::Button("IBL_Specular", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&ibl_specular);
			if(ImGui::Button("SSAO", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&ssao);
			if(ImGui::Button("SSAOHB", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&ssaohb);
			if(ImGui::Button("LTC", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&ltc);
			if(ImGui::Button("SH_Irradiance", ImVec2(-FLT_MIN, 0.0f)))changeDemo(&sh_irradiance);
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
	rLoop->exit();
	glfwTerminate();
	return 0;
}