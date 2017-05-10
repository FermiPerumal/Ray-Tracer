/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Material header class

***************************************/

#include "glm\glm\glm.hpp"

class Material
{
public:
	// Dark, medium, bright, border, specular, shadow colors
	Color c0, c1, c2, cb, cs, csh;
	// Transparency
	float tr;
	// Specularity, border, reflective, refractive
	bool isSpecular, isBorder, isReflective, isRefractive;
	// Texture Map
	ppmImage textureMap;
	// Texture Map
	ppmImage textureMap1;
	bool hasTexture;
	// Normal Map
	ppmImage normalMap;
	bool hasNormal;

	Material() {
	}

	Material(Color &c0, Color &c1, Color &c2, Color &csh, Color &cs, Color &cb, float tr, bool isSpecular, bool isBorder, bool isReflective, bool isRefractive)
		: c0(c0), c1(c1), c2(c2), csh(csh), cs(cs), cb(cb), tr(tr), isSpecular(isSpecular), isBorder(isBorder), isReflective(isReflective), isRefractive(isRefractive) {
		hasTexture = false;
		hasNormal = false;
		if (!isBorder)
		{
			//hasNormal = true;
			//normalMap = ppmImage("normal6.ppm");
			//isRefractive = false;
		}
	}
	
	Material(ppmImage &textureMap)
		: textureMap1(textureMap), textureMap(textureMap) {
		hasTexture = true;
		hasNormal = false;
		isSpecular = false;
		isReflective = false;
		isRefractive = false;
	}

	Material(ppmImage &textureMap1, ppmImage &textureMap)
		: textureMap1(textureMap1), textureMap(textureMap) {
		hasTexture = true;
		hasNormal = false;
		isSpecular = false;
		isReflective = false;
		isRefractive = false;
	}
	
	Material(ppmImage &textureMap1, ppmImage &textureMap, ppmImage &normalMap)
		: textureMap1(textureMap1), textureMap(textureMap), normalMap(normalMap) {
		hasTexture = hasNormal = true;
		isSpecular = false;
		isReflective = false;
		isRefractive = false;
	}
};
