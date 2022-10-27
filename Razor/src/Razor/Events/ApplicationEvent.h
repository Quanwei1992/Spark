#include "Event.h"
#include "Razor/Core.h"


#include <string>
#include <sstream>



namespace Razor
{
	class RAZOR_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class RAZOR_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width,unsigned height)
			:m_Width(width),m_Height(height)
		{}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};

	class RAZOR_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class RAZOR_API WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


	class RAZOR_API WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(unsigned int xPos, unsigned int yPos)
			:m_xPos(xPos), m_yPos(yPos)
		{}

		inline unsigned int GetXPos() const { return m_xPos; }
		inline unsigned int GetYPos() const { return m_yPos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << m_xPos << ", " << m_yPos;
			return ss.str();
		}


		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_xPos, m_yPos;
	};


	class RAZOR_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class RAZOR_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class RAZOR_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


}