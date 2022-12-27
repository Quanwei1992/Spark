#pragma once
#include "Application.h"


#ifdef SK_PLATFORM_WINDOWS

extern Spark::Application* Spark::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc,char** argv)
{
	Spark::Log::Init();
	SK_PROFILE_BEGIN_SESSION("Startup", "SparkProfile-Startup.json");
	Spark::Application* app = Spark::CreateApplication({ argc,argv });
	SK_PROFILE_END_SESSION();

	SK_PROFILE_BEGIN_SESSION("Runtime", "SparkProfile-Runtime.json");
	app->Run();
	SK_PROFILE_END_SESSION();

	SK_PROFILE_BEGIN_SESSION("Startup", "SparkProfile-Shutdown.json");
	delete app;
	SK_PROFILE_END_SESSION();

	return 0;
}


#endif
