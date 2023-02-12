#pragma once

#include "Spark/Core/Layer.h"

namespace Spark
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		 ~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		void Begin();
		void End();
		void BlockEvents(bool block) { m_BlockEvents = block; }
		virtual void OnImGuiRender() override;
		void SetDarkThemeColors();

	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = true;


	};
}