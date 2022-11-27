#pragma once

#include "Razor/Core/Layer.h"

namespace Razor
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		 ~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;

		void Begin();
		void End();

		virtual void OnImGuiRender() override;

	private:
		float m_Time = 0.0f;


	};
}