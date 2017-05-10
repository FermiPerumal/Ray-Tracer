/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Pixel header class

***************************************/

#include "Color.h"

#define WIDTH 1280
#define HEIGHT 720

class Pixel
{
public:
	Color* colors;
	Pixel()
	{
		colors = new Color[WIDTH * HEIGHT];
	}

	Pixel (int width, int height)
	{
		colors = new Color[width * height];
	}
};
