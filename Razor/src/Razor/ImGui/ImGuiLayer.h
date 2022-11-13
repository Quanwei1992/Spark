#pragma once

#include "Razor/Layer.h"

namespace Razor
{
	class RAZOR_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		 ~ImGuiLayer();

		void OnUpdate() override;
		void OnEvent(Event& event) override;

		void OnAttach() override;
		void OnDetach() override;

	private:
		float m_Time = 0.0f;


	};
}