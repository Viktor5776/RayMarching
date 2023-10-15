#include "App.h"
#include "../Utils/Vec2.h"
#include "../ImGui/imgui.h"
#include "../Utils/HydroTimer.h"
#include "../Utils/Random.h"

namespace Hydro
{
    App::App()
        :
        wnd( 1280, 720, L"Hydro Base" ),
        renderer( wnd.Gfx() ),
        camera( 90.0f, 0.1f, 100.0f ),
        scene()
        //scene( 
        //{
        //    {.pos{ 0.0f, -100.5f,0.0f }, .radius = 100.0f, .materialIndex = 0 },
        //    {.pos{ 0.0f,0.0f,0.0f}, .radius = 0.5f, .materialIndex = 1 },
        //    {.pos{ -1.0f,0.0f,0.0f }, .radius = 0.5f, .materialIndex = 2 },
        //    {.pos{ 1.0f,0.0f,0.0f}, .radius = 0.5f, .materialIndex = 3 },
        //}, 
        //{
        //    {.albedo{ 0.8f, 0.8f,0.0f}, .metalRoughness = 0.0f, .isMetal = false },
        //    {.albedo{ 0.7f, 0.3f,0.3f}, .metalRoughness = 0.0f, .isMetal = false },
        //    {.albedo{ 0.8f, 0.8f,0.8f}, .metalRoughness = 0.3f, .isMetal = true  },
        //    {.albedo{ 0.8f, 0.6f,0.2f}, .metalRoughness = 1.0f, .isMetal = true  },
        //})
	{
        comboBoxIndexObject = 0;
        comboBoxIndexMaterial = 0;
        //Init Scene
        scene.lightDir = { -1.0f,-1.0f,-1.0f };
        scene.ambient = 0.2f;

        scene.objectCount = 70;
        scene.materialCount = 70;

        scene.spheres[0] = { { 0.0f, -1000.f,0.0f }, 1000.0f, 0 };
        scene.materials[0] = { { 0.5f, 0.5f,0.5f }, 0.0f, false };

        scene.spheres[1] = { { 1, 1, 3 }, 1.0f, 1 };
        scene.materials[1] = { { 0.4, 0.2, 0.1 }, 0.0f, false };

        scene.spheres[2] = { { 4, 1, 0 }, 1.0f, 2 };
        scene.materials[2] = { { 0.7, 0.6, 0.5 }, 0.0f, true };

        int i = 2;

        for( int a = 0; a < 8; a++ ) {
            for( int b = 0; b < 8; b++ ) {
                i++;
                auto choose_mat = Hydro::Random::Float(0.0f,-1.0f);
                Vec3F center( a + 0.9 * Hydro::Random::Float( 0.0f, -1.0f ), 0.2, b + 0.9 * Hydro::Random::Float( 0.0f, -1.0f ) );

                if( (center - Vec3F( 4, 0.2, 0 )).Magnitude() > 0.9 ) {
                    
                    if( choose_mat < 0.88 ) {
                        // diffuse
                        auto albedo = Random::Vec3(0.0f,1.0f) * Random::Vec3( 0.0f, 1.0f );
                        scene.spheres[i] = { center, 0.2, i };
                        scene.materials[i] = Material{ .albedo = albedo, .metalRoughness = 0.0f, .isMetal = 0 };
                    }
                    else {
                        // metal
                        auto albedo = Random::Vec3( 0.5f, 1.0f );
                        auto fuzz = Random::Float( 0.0f, 0.5f );
                        scene.spheres[i] = { center, 0.2, i };
                        scene.materials[i] = Material{ .albedo = albedo, .metalRoughness = fuzz, .isMetal = 1 };
                    }
                }
            }
        }

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

        auto image = renderer.GetFinalImage();
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