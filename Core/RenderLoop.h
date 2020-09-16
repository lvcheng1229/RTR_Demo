#ifndef RENDERLOOP_H
#define RENDERLOOP_H

#include"LoadTexture.h"
#include"shader.h"
#include"RenderFixed.h"
#include"UltSetting.h"
#include"Model.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
class RenderLoop
{
public:
	string demoName;
	string GetDemoName() {
		return demoName;
	};

	void SetDemoName(string Name)
	{
		demoName = Name;
	}

	void changeDemo()
	{
		initDemo();
		SettingBeforLoop();
	}
	virtual void SettingBeforLoop() {};
	virtual void update() {};
	virtual void exit(){};
	virtual void DrawUI() {};
	virtual void initDemo() {};
	
};
#endif
