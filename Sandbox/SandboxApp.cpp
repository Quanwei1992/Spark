#include "MiniEngine.h"


class SandBoxApp : public Mini::Application
{

};


Mini::Application* Mini::CreateApplication()
{
	return new SandBoxApp();
}


