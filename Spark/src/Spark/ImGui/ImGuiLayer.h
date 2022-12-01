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

		void Begin();
		void End();

		virtual void OnImGuiRender() override;

	private:
		float m_Time = 0.0f;


	};
}