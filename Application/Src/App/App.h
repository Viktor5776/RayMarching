#pragma once
#include "../Win/Window.h"
#include "../Utils/HydroTimer.h"
#include "../Win/Image.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include <optional>
#include <vector>
#include <functional>

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

		int currentScene = 3;
		std::vector<std::function<Scene()>> scenes;

		Scene scene;
		std::optional<int> comboBoxIndexObject;
		std::optional<int> comboBoxIndexMaterial;

		float lastRenderTime = 0.0f;
	};
}