#pragma once
#include "../Utils/Vec3.h"
#include "../ImGui/imgui.h"
#include <optional>
#include <vector>

using namespace Hydro;

struct Sphere
{
    Vec3F pos = Vec3F( 0.0f );
	float radius = 0.0f;
    int materialIndex = 0;
    Vec3I padding;

	void SpawnControlWindow( int id, int materialCount )
	{
		using namespace std::string_literals;

		ImGui::Text( "Position" );
		ImGui::DragFloat3( "pos", &pos.x, 0.1f );
		ImGui::Text( "Radius" );
		ImGui::DragFloat( "radius", &radius, 0.1f );
        ImGui::Text( "Material" );
		ImGui::DragInt( "material",&materialIndex,0.1f,0,materialCount-1 );
		ImGui::Separator();
	}
};

struct Material
{
    Vec3F albedo;
    
    //Option
    float metalRoughness = 0.0f;
    int isMetal = 0;

    Vec3I padding;

    void SpawnControlWindow( int id )
    {
        using namespace std::string_literals;

        ImGui::Text( "Albedo" );
        ImGui::ColorEdit3( "Albedo",&albedo.x );

        bool isMetalBool = false;
        if( isMetal == 1 )
			isMetalBool = true;
        ImGui::Checkbox( "Metal",&isMetalBool );
        isMetal = isMetalBool ? 1 : 0;

        ImGui::Text( "Metal Roughness" );
		ImGui::DragFloat( "metalRoughness",&metalRoughness,0.02f,0.0f,1.0f );
        ImGui::Separator();
    }
};

static constexpr int MAX_OBJECTS = 128;

struct Scene
{
	Vec3F lightDir;
	float ambient = 0.0f;
    int objectCount = 0;
    int materialCount = 0;
    Vec2I padding;
	Sphere spheres[MAX_OBJECTS];
    Material materials[MAX_OBJECTS];
    
    

    Scene() = default;

    Scene( std::vector<Sphere> startSpheres, std::vector<Material> startMaterials )
        :
        objectCount( (int)startSpheres.size() ),
        materialCount( (int)startMaterials.size() )
    {
        for( int i = 0; i < objectCount; i++ )
		{
			spheres[i] = startSpheres[i];
		}

        for( int i = 0; i < materialCount; i++ )
        {
            materials[i] = startMaterials[i];
        }
    }

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
        if( ImGui::BeginCombo( "Object number", previewObject.c_str() ) )
        {
            for( int i = 0; i < objectCount; i++ )
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
            if( ImGui::Selectable( "Add Object" ) )
            {
                objectCount = ++objectCount > MAX_OBJECTS ? MAX_OBJECTS : objectCount;
                comboBoxIndexObject = objectCount-1;
            }
            if( ImGui::Selectable( "Remove Object") )
		    {
                objectCount = --objectCount < 0 ? 0 : objectCount;
			}


            ImGui::EndCombo();
        }
        if( comboBoxIndexObject.has_value() )
        {
            spheres[comboBoxIndexObject.value()].SpawnControlWindow( comboBoxIndexObject.value(), materialCount );
        }
        ImGui::NewLine();
        ImGui::Text( "Materials" );
        ImGui::Separator();
        const auto previewMaterial = comboBoxIndexMaterial ? std::to_string( *comboBoxIndexMaterial ) : "Choose a material..."s;
        if( ImGui::BeginCombo( "Material Number", previewMaterial.c_str() ) )
        {
            for( int i = 0; i < materialCount; i++ )
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
            if( ImGui::Selectable( "Add Material" ) )
            {
                materialCount = ++materialCount > MAX_OBJECTS ? MAX_OBJECTS : materialCount;
                comboBoxIndexMaterial = materialCount - 1;
            }
            if( ImGui::Selectable( "Remove Material" ) )
            {
                materialCount = --materialCount < 0 ? 0 : materialCount;
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