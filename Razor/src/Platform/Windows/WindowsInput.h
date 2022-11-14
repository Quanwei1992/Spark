#pragma once

#include "Razor/Events/Input.h"



namespace Razor
{
	class WindowsInput : public Input
	{
	protected:
		 virtual bool IsKeyPressedImpl(int keycode) override;
		 virtual bool IsMouseButtonPressedImpl(int button) override;
		 virtual float GetMouseXImpl() override;
		 virtual float GetMouseYImpl() override;
		 virtual std::pair<float, float> GetMousePositionImpl() override;
	};
}