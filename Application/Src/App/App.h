#pragma once
#include "../Win/Window.h"
#include "../Utils/HydroTimer.h"
#include "../Win/Image.h"
#include <memory>

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
		void Render();
		void RenderImGuiBaseGUI();
	private:
		Timer dt;
		Window wnd;
		uint32_t ViewportWidth = 0, ViewportHeight = 0;
		
		Image image;
		uint32_t* ImageData = nullptr;

		float lastRenderTime = 0.0f;
	};
}