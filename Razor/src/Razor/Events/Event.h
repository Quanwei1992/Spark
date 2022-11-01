#pragma once
#include "rzpch.h"
#include "Razor/Core.h"

namespace Razor
{
	enum class EventType
	{
		None = 0,
		// Window 
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,		
		// App
		AppTick,
		AppUpdate,
		AppRender,
		// Key
		KeyPressed,
		KeyReleased,
		// Mouse
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	enum EventCategory
	{
		EventCategoryNone			= 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};



	class RAZOR_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategoty(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}


		template<typename OStream>
		friend OStream& operator<<(OStream& os, const Event& c)
		{
			return os << "[" << c.GetName() << "Event] " << c.ToString();
		}
	protected:
		bool m_Handled = false;
	};

	/*template <typename T>
	struct fmt::formatter<T, std::enable_if_t<std::is_base_of<Event, T>::value, char>> :
		fmt::formatter<std::string> {
		template <typename FormatCtx>
		auto format(const A& a, FormatCtx& ctx) const {
			return fmt::formatter<std::string>::format(a.ToString(), ctx);
		}
	};*/


	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			:m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};





#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::##type; }\
							    virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
}