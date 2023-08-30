#pragma once
#include <vector>
#include <string>
#include "Color.h"

namespace Hydro
{
	class Texture
	{
	public:
		Texture( int width, int height );
		Texture( std::string path );
		~Texture();
		uint32_t* GetData() { return pixels; }
		void ScaleTexture( int width, int height );
		int GetWidth() const;
		int GetHeight() const;
	private:
		int width;
		int height;
		uint32_t* pixels;
	};
}