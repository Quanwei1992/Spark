#pragma once
#include "Application.h"


#ifdef RZ_PLATFORM_WINDOWS

extern Razor::Application* Razor::CreateApplication();

int main(int argc,const char** argv)
{
	Razor::Log::Init();
	RZ_CORE_WARN("Initiazlied Log!");
	int a = 666;
	RZ_INFO("Hello Var={0}!",a);
	Razor::Application* app = Razor::CreateApplication();
	app->Run();
	delete app;
	return 0;
}


#endif