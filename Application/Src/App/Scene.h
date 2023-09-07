#pragma once
#include "../Utils/Vec3.h"
#include "../ImGui/imgui.h"
#include <optional>
using namespace Hydro;

struct Sphere
{
    Vec3F pos = Vec3F( 0.0f );
	float radius = 0.0f;
    int materialIndex = 0;

	void SpawnControlWindow( int id )
	{
		using namespace std::string_literals;

		ImGui::Text( "Position" );
		ImGui::DragFloat3( "pos", &pos.x, 0.1f );
		ImGui::Text( "Radius" );
		ImGui::DragFloat( "radius", &radius, 0.1f );
        ImGui::Text( "Material" );
		ImGui::DragInt( "material",&materialIndex,0.1f,0,9 );
		ImGui::Separator();
	}
};

struct Material
{
    Vec3F albedo;
    float roughness;
    float metallic;
    //Emission

    void SpawnControlWindow( int id )
    {
        using namespace std::string_literals;

        ImGui::Text( "Position" );
        ImGui::ColorEdit3( "pos",&albedo.x,0.1f );
        ImGui::Text( "Roughness" );
        ImGui::DragFloat( "roughness",&roughness,0.05f,0.0f,1.0f );
        ImGui::Text( "Metallic" );
        ImGui::DragFloat( "metallic",&metallic,0.05f,0.0f,1.0f );
        ImGui::Separator();
    }
};

struct Scene
{
	Vec3F lightDir;
	float ambient;
	Sphere spheres[10];
    Material materials[10];

	void RenderGUI( std::optional<int>& comboBoxIndex )
	{
        ImGui::Begin( "Scene" );
        ImGui::SliderFloat3( "Light Direction", &lightDir.x, -1.0f, 1.0f );
        ImGui::SliderFloat( "Ambient", &ambient, 0.0f, 1.0f );
        using namespace std::string_literals;
        ImGui::NewLine();
        ImGui::Separator();
        const auto preview = comboBoxIndex ? std::to_string( *comboBoxIndex ) : "Choose a shpere..."s;
        if( ImGui::BeginCombo( "Sphere number", preview.c_str() ) )
        {
            for( int i = 0; i < 10; i++ )
            {
                const bool selected = comboBoxIndex ? *comboBoxIndex == i : false;
                if( ImGui::Selectable( std::to_string( i ).c_str(), selected ) )
                {
                    comboBoxIndex = i;
                }
                if( selected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if( comboBoxIndex.has_value() )
        {
            spheres[comboBoxIndex.value()].SpawnControlWindow( comboBoxIndex.value() );
        }
        ImGui::End();
	}
};