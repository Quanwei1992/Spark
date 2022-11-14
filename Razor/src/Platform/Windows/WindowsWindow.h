#include "Razor/Core.h"
#include "Razor/Window.h"

struct GLFWwindow;
namespace Razor
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;
		inline virtual unsigned int GetWidth() const override { return m_Data.Width; }
		inline virtual unsigned int GetHeight() const override { return m_Data.Height; }
		virtual void SetEventCallback(const EventCallbackFn& callback) override;
		virtual void SetVsync(bool enabled) override;
		virtual bool IsVsync() const override;
		inline virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window = nullptr;
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
	};
}