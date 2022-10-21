#pragma once
#include "Application.h"


#ifdef ME_PLATFORM_WINDOWS

extern Mini::Application* Mini::CreateApplication();

int main(int argc,const char** argv)
{
	printf("Hello world");
	Mini::Application* app = Mini::CreateApplication();
	app->Run();
	delete app;
	return 0;
}


#endif