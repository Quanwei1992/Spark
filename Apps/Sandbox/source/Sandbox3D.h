#pragma once
#include <Spark.h>
class Sandbox3D : public Spark::Layer
{
public:
	Sandbox3D();
	virtual ~Sandbox3D();
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Spark::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Spark::Event& event)override;
};
