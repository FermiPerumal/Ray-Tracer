/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Main function for the ray tracer

***************************************/


#include<fstream>
#include<vector>
#include<algorithm>
#include <string>
#include <sstream>
#include "objLoader.h"
#include "RayTrace.h"

int main(int argc, char **argv)
{

	vector<Light*> lights;
	vector<Object*> objects;

	// Dark, medium, bright, shadow, specular, border, transparency, specularity, border, reflective, refractive
	Material m1(WALL0, WALL1, WALL2, WALLSH, CNULL, CNULL, 1.0f, false, false, true, false);
	Material m0(CLOTH0, CLOTH1, CLOTH2, CLOTHSH, CNULL, CNULL, 1.0f, true, true, true, false);

	// Material with texture maps
	Material mt0(ppmImage("abstract_b.ppm"));
	Material mt1(ppmImage("mahal_b.ppm"));
	Material mt1(ppmImage("pink2.ppm"), ppmImage("pink4.ppm"), ppmImage("marn.ppm"));
	// View direction, up direction and eye position

	Camera cam(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(6.5,0,70), 4, WIDTH, HEIGHT, 8);

	// Normal, point and material
	Plane plane(glm::vec3(0, 0, -1), glm::vec3(0, 0, 20), m1);
	Plane plane1(glm::vec3(0, 0, 1), glm::vec3(0, 0, -35), m0);
	Sphere sphere(glm::vec3(-8, 10, 0), 5.0f, mt0); // In focus

	Sphere sphere1(glm::vec3(0, 0, 6), 10.0f, m1);
	Sphere sphere2(glm::vec3(-3, 1, 16), 1.3f, m0);
	Sphere sphere3(glm::vec3(2, 2, 2), 1.0f, m0);
	Mesh mesh("cube_b.obj", m0);

	// Light defined by direction and position
	PointLight pLight(glm::vec3(0, 0, 0), glm::vec3(5, 5, -25));
	DirectionalLight dLight(glm::vec3(-1, 1, 1));
	SpotLight sLight(glm::vec3(0, 1, 1), glm::vec3(5, 5, -5));
	LineLight lLight(glm::vec3(1, 0, 0), glm::vec3(0, -1, 7));

	lights.push_back((Light*)&pLight);

	objects.push_back((Object*)&sphere);
	objects.push_back((Object*)&sphere1);
	objects.push_back((Object*)&sphere2);
	objects.push_back((Object*)&sphere3);
	objects.push_back((Object*)&plane1);
	objects.push_back((Object*)&mesh);

	RayTrace rayTracer(cam, objects, lights);
	float px = 0, pz = 0;
	
	float animateSphere = 0.5f;
	float startFrame = 41, endFrame = 60;
	float stepSize = 360.0 / (endFrame - startFrame);
	float angle = 0;
	float sior = 1.0, lior = 40.0;
	float iorStep = (lior - sior) / (lior * 10.0);
	float ior = 0.5;

	for (int frame = startFrame; frame <=endFrame; frame = frame + 1)
	{
		rayTracer.render(frame, sphere1.pi, sphere1.r, ior);
	}
}

