#include "module.h"

#include <glew/glew.h>
#include "dependency/sfml/Graphics.hpp"
using namespace sf;


GLuint ModuleTerrain::Texture()
{
	Image image;
	bool result = image.loadFromFile("module/" + Name() + "/texture/terrain.png");
	if(!result)
	{
		Log->Fail("texture loading fail");
		return 0;
	}

	// generate texture
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

	// cut tiles
	const Vector2u TILESIZE(image.getSize().x / TILES_U, image.getSize().y / TILES_V);
	Image tiles[TILES_U][TILES_V];
	for(unsigned int u = 0; u < TILES_U; ++u)
	for(unsigned int v = 0; v < TILES_V; ++v)
	{
		tiles[u][v].create(TILESIZE.x, TILESIZE.y);
		tiles[u][v].copy(image, 0, 0, IntRect(u * TILESIZE.x, v * TILESIZE.y, (u+1) * TILESIZE.x, (v+1) * TILESIZE.y), true);
	}

	// generate mipmaps
	Vector2u size = Vector2u(TILESIZE.x / 2, TILESIZE.y / 2);
	int level;
	for(level = 0; size.x > 1 && size.y > 1; size.x /= 2, size.y /= 2)
	{
		level++;

		Image mipmap;
		mipmap.create(size.x * TILES_U, size.y * TILES_V);
		for(int u = 0; u < TILES_U; ++u)
		for(int v = 0; v < TILES_V; ++v)
		{
			TextureShrink(tiles[u][v]);
			mipmap.copy(tiles[u][v], u * size.x, v * size.y, IntRect(0, 0, 0, 0), true);
		}
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, mipmap.getSize().x, mipmap.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipmap.getPixelsPtr());
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);

	glBindTexture(GL_TEXTURE_2D, 0);
	return id;
}

void ModuleTerrain::TextureShrink(Image &Texture)
{
	Vector2u size(Texture.getSize().x / 2, Texture.getSize().y / 2);

	Image half;
	half.create(size.x, size.y);
	for(unsigned int x = 0; x < size.x; ++x)
	for(unsigned int y = 0; y < size.y; ++y)
	{
		Color input[4];
		input[0] = Texture.getPixel(2 * x + 0, 2 * y + 0);
		input[1] = Texture.getPixel(2 * x + 1, 2 * y + 0);
		input[2] = Texture.getPixel(2 * x + 0, 2 * y + 1);
		input[3] = Texture.getPixel(2 * x + 1, 2 * y + 1);
		half.setPixel(x, y, TextureAverage(input[0], input[1], input[2], input[3]));
	}
	Texture = half;
}

Color ModuleTerrain::TextureAverage(Color A, Color B, Color C, Color D)
{
	return Color
	(
		((A.r + B.r + C.r + D.r) / 4),
		((A.g + B.g + C.g + D.g) / 4),
		((A.b + B.b + C.b + D.b) / 4),
		((A.a + B.a + C.a + D.a) / 4)
	);
}
