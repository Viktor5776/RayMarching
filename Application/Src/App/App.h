#pragma once
#include "../Win/Window.h"
#include "../Utils/HydroTimer.h"


namespace Hydro
{
	class App
	{
	public:
		App();
		~App();
		int Run();
	private:
		void Update();
		void Frame();
		void RenderImGuiBaseGUI();
	private:
		Timer dt;
		Window wnd;
		float lastRenderTime = 0.0f;
	};
}