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
    Vec3I padding;

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
    float padding;
    //Emission

    void SpawnControlWindow( int id )
    {
        using namespace std::string_literals;

        ImGui::Text( "Albedo" );
        ImGui::ColorEdit3( "Albedo",&albedo.x );
        ImGui::Separator();
    }
};

struct Scene
{
	Vec3F lightDir;
	float ambient;
	Sphere spheres[10];
    Material materials[10];

	void RenderGUI( std::optional<int>& comboBoxIndexObject, std::optional<int>& comboBoxIndexMaterial )
	{
        using namespace std::string_literals;
        ImGui::Begin( "Scene" );
        ImGui::SliderFloat3( "Light Direction", &lightDir.x, -1.0f, 1.0f );
        ImGui::SliderFloat( "Ambient", &ambient, 0.0f, 1.0f );
        ImGui::NewLine();
        ImGui::Text( "Objects" );
        ImGui::Separator();
        const auto previewObject = comboBoxIndexObject ? std::to_string( *comboBoxIndexObject ) : "Choose a object..."s;
        if( ImGui::BeginCombo( "Sphere number", previewObject.c_str() ) )
        {
            for( int i = 0; i < 10; i++ )
            {
                const bool selected = comboBoxIndexObject ? *comboBoxIndexObject == i : false;
                if( ImGui::Selectable( std::to_string( i ).c_str(), selected ) )
                {
                    comboBoxIndexObject = i;
                }
                if( selected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if( comboBoxIndexObject.has_value() )
        {
            spheres[comboBoxIndexObject.value()].SpawnControlWindow( comboBoxIndexObject.value() );
        }
        ImGui::NewLine();
        ImGui::Text( "Materials" );
        ImGui::Separator();
        const auto previewMaterial = comboBoxIndexMaterial ? std::to_string( *comboBoxIndexMaterial ) : "Choose a material..."s;
        if( ImGui::BeginCombo( "Material Number", previewMaterial.c_str() ) )
        {
            for( int i = 0; i < 10; i++ )
            {
                const bool selected = comboBoxIndexMaterial ? *comboBoxIndexMaterial == i : false;
                if( ImGui::Selectable( std::to_string( i ).c_str(), selected ) )
                {
                    comboBoxIndexMaterial = i;
                }
                if( selected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if( comboBoxIndexMaterial.has_value() )
        {
            materials[comboBoxIndexMaterial.value()].SpawnControlWindow( comboBoxIndexMaterial.value() );
        }
        ImGui::End();
	}
};