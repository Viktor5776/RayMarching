#include "Texture.h"
#include <fstream>
#include <assert.h>
#include <math.h>

namespace Hydro
{

	Texture::Texture( int width, int height )
		:
		width( width ),
		height( height )
	{
		pixels = new uint32_t[width * height];
	}

	Texture::Texture( std::string path )
		:
		width( 0 ),
		height( 0 )
	{
		std::ifstream f;
		f.open( path, std::ios::in | std::ios::binary );

		assert( f.is_open() );

		const int fileHeaderSize = 14;
		const int informationHeaderSize = 40;

		unsigned char fileHeader[fileHeaderSize];
		f.read( reinterpret_cast<char*>(fileHeader), fileHeaderSize );

		assert( fileHeader[0] == 'B' && fileHeader[1] == 'M' );

		unsigned char informationHeader[informationHeaderSize];
		f.read( reinterpret_cast<char*>(informationHeader), informationHeaderSize );

		int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
		width = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
		height = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

		pixels = new uint32_t[width * height];

		const int paddingAmout = ((4 - (width * 3) % 4) % 4);

		for( int y = height - 1; y >= 0; y-- )
		{
			for( int x = 0; x < width; x++ )
			{
				unsigned char color[3];
				f.read( reinterpret_cast<char*>(color), 3 );

				pixels[y * width + x] = (255 << 24) | (color[0] << 16) | (color[1] << 8) | color[2];
			}

			f.ignore( paddingAmout );
		}
		
		f.close();
	}

	Texture::~Texture()
	{
		delete[] pixels;
	}

	void Texture::ScaleTexture( int widthScaled, int heightScaled )
	{
		assert( widthScaled > 0 );
		assert( heightScaled > 0 );

		uint32_t* scaledPixels = new uint32_t[widthScaled * heightScaled];

		float xDelta = width / (float)widthScaled;
		float yDelta = height / (float)heightScaled;

		for( int y = 0; y < heightScaled; y++ )
		{
			for( int x = 0; x < widthScaled; x++ )
			{
				scaledPixels[y * widthScaled + x] = pixels[((int)(y * yDelta)) * width + (int)(x * xDelta)];
			}
		}

		width = widthScaled;
		height = heightScaled;
		delete[] pixels;
		pixels = scaledPixels;

	}

	int Texture::GetWidth() const
	{
		return width;
	}

	int Texture::GetHeight() const
	{
		return height;
	}

}