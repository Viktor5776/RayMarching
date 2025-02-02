#include "App.h"
#include "../Utils/Vec2.h"
#include "../ImGui/imgui.h"
#include "../Utils/HydroTimer.h"
#include "../Utils/Random.h"

Scene Scene_Sphere( Renderer& renderer )
{
    Scene scene;

    scene.objectCount = 1;
    scene.materialCount = 1;

    scene.objects[0].active = 1;
    scene.objects[0].id = 0;
    scene.objects[0].materialIndex = 0;

    scene.objects[0].data[0] = 0.0f;
    scene.objects[0].data[1] = 0.0f;
    scene.objects[0].data[2] = 0.0f;
    scene.objects[0].data[3] = 0.5f;

    scene.materials[0].data[0] = 0.8f;
    scene.materials[0].data[1] = 0.8f;
    scene.materials[0].data[2] = 0.0f;

    renderer.SetSkybox( "Src/App/Textures/Skybox.bmp" );

    return scene;
}


Scene Scene_Cube( Renderer& renderer )
{
	Scene scene;

	scene.objectCount = 1;
	scene.materialCount = 1;

	scene.objects[0].active = 1;
	scene.objects[0].id = 1;
	scene.objects[0].materialIndex = 0;

	scene.objects[0].data[0] = 0.0f;
	scene.objects[0].data[1] = 0.0f;
	scene.objects[0].data[2] = 0.0f;
	scene.objects[0].data[3] = 0.5f;
    scene.objects[0].data[4] = 0.5f;
    scene.objects[0].data[5] = 0.5f;

	scene.materials[0].data[0] = 0.0f;
	scene.materials[0].data[1] = 0.6f;
	scene.materials[0].data[2] = 0.8f;

	renderer.SetSkybox( "Src/App/Textures/Skybox.bmp" );

	return scene;
}



Scene Scene_Torus( Renderer& renderer )
{
	Scene scene;

	scene.objectCount = 1;
	scene.materialCount = 1;

	scene.objects[0].active = 1;
	scene.objects[0].id = 2;
	scene.objects[0].materialIndex = 0;

	scene.objects[0].data[0] = 0.0f;
	scene.objects[0].data[1] = 0.0f;
	scene.objects[0].data[2] = 0.0f;
	scene.objects[0].data[3] = 0.5f;
	scene.objects[0].data[4] = 0.2f;

	scene.materials[0].data[0] = 0.8f;
	scene.materials[0].data[1] = 0.0f;
	scene.materials[0].data[2] = 0.8f;

	renderer.SetSkybox( "Src/App/Textures/Skybox.bmp" );

	return scene;
}

Scene Scene_CornellBox( Renderer& renderer )
{
    Scene scene;

    scene.objectCount = 6;
    scene.materialCount = 4;

    //Red
    scene.materials[0].id = 0;
    scene.materials[0].data[0] = 0.65f;
    scene.materials[0].data[1] = 0.05f;
    scene.materials[0].data[2] = 0.05f;

    //White
    scene.materials[1].id = 0;
    scene.materials[1].data[0] = 0.73f;
    scene.materials[1].data[1] = 0.73f;
    scene.materials[1].data[2] = 0.73f;

    //Green
    scene.materials[2].id = 0;
    scene.materials[2].data[0] = 0.12f;
    scene.materials[2].data[1] = 0.45f;
    scene.materials[2].data[2] = 0.15f;

    //Light
    scene.materials[3].id = 0;
    scene.materials[3].data[0] = 1.0f;
    scene.materials[3].data[1] = 1.0f;
    scene.materials[3].data[2] = 1.0f;
    scene.materials[3].emitedLight = Vec3F( 1.0f, 1.0f, 1.0f );
    scene.materials[3].data[15] = 15.0f;

    //Left Wall
    scene.objects[0].active = 1;
    scene.objects[0].id = 1;
    scene.objects[0].materialIndex = 2;
    scene.objects[0].data[0] = -3.0f;
    scene.objects[0].data[1] = 0.0f;
    scene.objects[0].data[2] = 0.5f;
    scene.objects[0].data[3] = 0.1f;
    scene.objects[0].data[4] = 2.5f;
    scene.objects[0].data[5] = 3.0f;

    //Right Wall
    scene.objects[1].active = 1;
    scene.objects[1].id = 1;
    scene.objects[1].materialIndex = 0;
    scene.objects[1].data[0] = 3.0f;
    scene.objects[1].data[1] = 0.0f;
    scene.objects[1].data[2] = 0.5f;
    scene.objects[1].data[3] = 0.1f;
    scene.objects[1].data[4] = 2.5f;
    scene.objects[1].data[5] = 3.0f;

    //Back Wall
    scene.objects[2].active = 1;
    scene.objects[2].id = 1;
    scene.objects[2].materialIndex = 1;
    scene.objects[2].data[0] = 0.0f;
    scene.objects[2].data[1] = 0.0f;
    scene.objects[2].data[2] = -2.6f;
    scene.objects[2].data[3] = 2.9f;
    scene.objects[2].data[4] = 2.5f;
    scene.objects[2].data[5] = 0.1f;

    //Floor
    scene.objects[3].active = 1;
    scene.objects[3].id = 1;
    scene.objects[3].materialIndex = 1;
    scene.objects[3].data[0] = 0.0f;
    scene.objects[3].data[1] = -2.6f;
    scene.objects[3].data[2] = 0.5f;
    scene.objects[3].data[3] = 2.9f;
    scene.objects[3].data[4] = 0.1f;
    scene.objects[3].data[5] = 3.0f;

    //Ceiling
    scene.objects[4].active = 1;
    scene.objects[4].id = 1;
    scene.objects[4].materialIndex = 1;
    scene.objects[4].data[0] = 0.0f;
    scene.objects[4].data[1] = 2.6f;
    scene.objects[4].data[2] = 0.5f;
    scene.objects[4].data[3] = 2.9f;
    scene.objects[4].data[4] = 0.1f;
    scene.objects[4].data[5] = 3.0f;

    //Ceiling Light
    scene.objects[5].active = 1;
    scene.objects[5].id = 1;
    scene.objects[5].materialIndex = 3;
    scene.objects[5].data[0] = 0.0f;
    scene.objects[5].data[1] = 2.59f;
    scene.objects[5].data[2] = 0.5f;
    scene.objects[5].data[3] = 0.5f;
    scene.objects[5].data[4] = 0.1f;
    scene.objects[5].data[5] = 0.5f;

    renderer.SetSkybox( "Src/App/Textures/NoSkybox.bmp" );
    return scene;
}

namespace Hydro
{
    App::App()
        :
        wnd( 1280, 720, L"Hydro Base" ),
        renderer( wnd.Gfx() ),
        camera( 90.0f, 0.1f, 100.0f )
	{
        comboBoxIndexObject = 0;
        comboBoxIndexMaterial = 0;

        //Init Scenes
        scenes.push_back( std::bind( Scene_Sphere, std::ref( renderer ) ) );
        scenes.push_back( std::bind( Scene_Cube, std::ref( renderer ) ) );
        scenes.push_back( std::bind( Scene_Torus, std::ref( renderer ) ) );
        scenes.push_back( std::bind( Scene_CornellBox, std::ref( renderer ) ) );

        scene = scenes[currentScene]();
	}

	App::~App()
	{}

    void App::Update()
    {
        float deltaTime = dt.Mark();
        camera.OnUpdate( wnd, deltaTime );
    }

    void App::Frame()
	{
		wnd.Gfx().BeginFrame();
		
		RenderImGuiBaseGUI();
        
        scene.RenderGUI( comboBoxIndexObject, comboBoxIndexMaterial );

        ImGui::Begin( "Settings" );
        ImGui::DragInt( "Scene", &currentScene, 0.5f, 0, scenes.size() - 1 );
        if( ImGui::Button( "Change Scene" ) )
        {
            scene = scenes[currentScene]();
        }
        ImGui::Text( "Last render time: %.3fms", lastRenderTime * 1000 );
        ImGui::Text( "Fps: %.1f", ImGui::GetIO().Framerate );
        ImGui::NewLine();
        ImGui::InputInt("Render iterations", &renderer.GetRenderIterations(), 1, 10); 
        if( ImGui::Button( "Render" ) )
        {
            Render();
        }
        static bool render = true;
        ImGui::Checkbox( "Live Render", &render );
        if( render )
        {
            Render();
        }
        ImGui::End();

        //Render
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::Begin( "Viewport" );
		
        ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
        ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

        Hydro::Image image = renderer.GetFinalImage();
        if( image.Active() )
        {
            ImGui::Image( image.GetData(), { (float)image.GetWidth(),(float)image.GetHeight() },
                ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
        }

        ImGui::End();
        ImGui::PopStyleVar();

        wnd.Gfx().EndFrame();
	}

    void App::Render()
    {
        Timer timer;

        renderer.OnResize( ViewportWidth, ViewportHeight );
        camera.OnResize( ViewportWidth, ViewportHeight );
        renderer.Render( camera, scene );

        lastRenderTime = timer.Mark();
    }

	void App::RenderImGuiBaseGUI()
	{
		//Dockspace 
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if( opt_fullscreen )
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos( viewport->WorkPos );
            ImGui::SetNextWindowSize( viewport->WorkSize );
            ImGui::SetNextWindowViewport( viewport->ID );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if( dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode )
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if( !opt_padding )
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGui::Begin( "DockSpace Demo", nullptr, window_flags );
        if( !opt_padding )
            ImGui::PopStyleVar();

        if( opt_fullscreen )
            ImGui::PopStyleVar( 2 );

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if( io.ConfigFlags & ImGuiConfigFlags_DockingEnable )
        {
            ImGuiID dockspace_id = ImGui::GetID( "MyDockSpace" );
            ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );
        }

        if( ImGui::BeginMenuBar() )
        {
            if( ImGui::BeginMenu( "Options" ) )
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem( "Fullscreen", NULL, &opt_fullscreen );
                ImGui::MenuItem( "Padding", NULL, &opt_padding );
                ImGui::Separator();

                if( ImGui::MenuItem( "Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0 ) ) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                if( ImGui::MenuItem( "Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0 ) ) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if( ImGui::MenuItem( "Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0 ) ) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                if( ImGui::MenuItem( "Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0 ) ) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if( ImGui::MenuItem( "Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen ) ) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
	}


	int App::Run()
	{
		//Application Loop
		while( true )
		{ 
			//Process all window messages
			if( const auto code = Window::ProcessMessages() )
			{
				//Return exit code
				return *code;
			}
            Update();
			Frame();
		}
	}

}