#pragma once

#include "Razor/Core.h"
#include "Razor/Events/Event.h"
#include "Razor/Core/Timestep.h"

namespace Razor
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep st) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}