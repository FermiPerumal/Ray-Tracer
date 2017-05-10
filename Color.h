/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Color header class

***************************************/

#include "glm\glm\glm.hpp"

class Color {
public:
	unsigned char r, g, b;

	Color()
	{
	}

	Color(unsigned char r0, unsigned char g0, unsigned char b0)
	{
		r = r0;
		g = g0;
		b = b0;
	}

	void operator = (glm::vec3 v)
	{
		r = v.x;
		g = v.y;
		b = v.z;
	}

	Color operator + (Color c1)
	{
		Color c;
		c.r = r + c1.r;
		c.g = g + c1.g;
		c.b = b + c1.b;
		return c;
	}

	Color operator - (Color c1)
	{
		Color c;
		c.r = r - c1.r;
		c.g = g - c1.g;
		c.b = b - c1.b;
		return c;
	}

	Color operator * (Color c1)
	{
		Color c;
		c.r = r * c1.r;
		c.g = g * c1.g;
		c.b = b * c1.b;
		return c;
	}

	glm::vec3 operator + (glm::vec3 c1)
	{
		glm::vec3 c;
		c.x = r + c1.x;
		c.y = g + c1.y;
		c.z = b + c1.z;
		return c;
	}

	Color operator /(int value)
	{
		Color c;
		c.r = r/value;
		c.g = g/value;
		c.b = b/value;
		return c;
	}

	Color operator *(float value) const
	{
		return Color(r * value, g * value, b * value);
	}
};

Color WHITE(255, 255, 255);
Color BLACK(0, 0, 0);
Color RED(255, 0, 0);
Color GREEN(0, 255, 0);
Color BLUE(0, 0, 255);
Color LIGHTLILAC(255, 186, 186);
Color DARKLILAC(255, 166, 166);
Color PICTIONBLUE(86, 186, 236);
Color BABYBLUE(180, 216, 231);
Color YELLOWGREEN(176, 229, 124);
Color LIGHTYELLOW(255, 240, 170);
Color DARKYELLOW(255, 236, 148);
Color LIGHTGREY(192, 192, 192);
Color DARKGREY(96, 96, 96);
Color AZUREBLUE(51, 102, 153);
Color BLACKCURRENT(61, 50, 66);
Color MIDNIGHTBLUE(0, 0, 68);

Color LIGHT(216, 179, 109);
Color DARK(145, 82, 67);
Color MEDIUM(184, 119, 99);
Color CNULL(-1, -1, -1);

// Image colors
// Wall
Color WALL0(77, 53, 19);
Color WALL1(83, 57, 20);
Color WALL2(84, 58, 25);
Color WALLSH(33, 29, 26);

// Pink Cloth
Color CLOTH0(128, 79, 75);
Color CLOTH1(120, 67, 61);
Color CLOTH2(231, 181, 122);
Color CLOTHS(249, 182, 137);
Color CLOTHB(225, 162, 111);
Color CLOTHSH(119, 67, 56);
