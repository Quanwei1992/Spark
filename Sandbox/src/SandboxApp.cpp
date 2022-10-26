#include "Razor.h"


class SandBoxApp : public Razor::Application
{

};


Razor::Application* Razor::CreateApplication()
{
	return new SandBoxApp();
}


