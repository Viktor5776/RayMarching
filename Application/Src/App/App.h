#pragma once
#include "../Win/Window.h"
#include "../Utils/HydroTimer.h"
#include "../Win/Image.h"
#include "Renderer.h"
#include "Camera.h"

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
		Renderer renderer;
		Camera camera;
		uint32_t ViewportWidth = 0, ViewportHeight = 0;
		
		float lastRenderTime = 0.0f;
	};
}