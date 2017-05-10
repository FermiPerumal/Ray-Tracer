/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Ray tracer header class

***************************************/

#include "glm\glm\glm.hpp"
#include "Pixel.h"
#include "PPMProcessor.h"
#include "Camera.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Quadric.h"

using namespace std;

#define M 3
#define N 3
#define ANTIALIAS false
#define NUM_OF_QUADRICS 8
#define MAXBOUNCE 5
#define PI 180
#define DOF false
#define CAMPAINT true
#define MOTIONBLUR false

class RayTrace
{
public:
	Camera cam;
	Plane planes[1];
	Sphere* spheres;
	vector<Object*> objects;
	vector<Light*> lights;
	Pixel pixmap;
	Sphere* envDome;
	const float zero = 0.0f;
	float camPaint = 0.0f;
	Material *camPaintMap;
	RayTrace() {
	}

	RayTrace(Camera &c, vector<Object*>& objects, vector<Light*>& lights)
		: cam(c), objects(objects), lights(lights) {
		Material mat(ppmImage("bluebell_b.ppm"));
		camPaintMap = new Material(ppmImage("rain.ppm"));
		spheres = dynamic_cast<Sphere*>(objects[0]);
		envDome = new Sphere(glm::vec3(0, 0, 0), 5.0f, mat);
	}

	Color raytrace(glm::vec3 position, glm::vec3 direction, int bounce, float ior)
	{
		Color pixelColor = BLACK;
		if (bounce > MAXBOUNCE)
			return pixelColor;
		int triIndex = -1;
		Object *rayObject = NULL;
		glm::vec3 ph; // Point of intersection of the ray from eye point to object
		
		float th = INT_MAX, t;

		// Raytrace quadrics in the scene
		for each(Object* object in objects)
		{
			t = object->intersect(position, direction, triIndex);
			if (t >= 0 && t < th) {
				th = t; // Closest object is chosen
				rayObject = object;
			}
		}
		// Find color for the intersection point
		if (rayObject != NULL)
		{
			ph = position + direction  * th;
			pixelColor = getColor(rayObject, bounce, ph, direction, cam, lights, objects, triIndex, ior);
		}
		else
			pixelColor = pixelColor + envDome->getSkyColor(direction);
		return pixelColor;
	}

	Color getColor(Object* currentObj, int bounce, glm::vec3 ph, glm::vec3 dir, Camera &cam, vector<Light*> &lights, vector<Object*>& objects, int &index, float &ior)
	{
		Material material = currentObj->material;
		Color c = material.c0;
		glm::vec3 nh = currentObj->normal(ph, index); // Normal at the intersection point ph; Index is for the triangle index when dealing with meshes
		glm::vec3 pl(0, 0, 0), nl(0, 0, 0);
		glm::vec3 nlh(0, 0, 0), plh(0, 0, 0);
		glm::vec3 n2(0, 0, 0), n0(1, 0, 0), n1(0, 0, 0);
		float cos0, t;
		// LIGHTS
		for each(Light* light in lights)
		{
			pl = light->position();
			nl = light->direction();

			// DIFFUSE
			if (light->type() == typeid(PointLight))
			{
				plh = pl - ph;
				nlh = glm::normalize(plh); // Normal from center of light to the intersection point ph
				cos0 = (glm::dot(nh, nlh) + 1) / 2;
				t = cos0;
			}
			else if (light->type() == typeid(SpotLight))
			{
				plh = pl - ph;
				nlh = glm::normalize(plh);
				float cos01 = glm::dot(nl, nh), cos02 = glm::dot(nlh, nl);
				t = ((cos01 + 1) / 2) * ((cos02 + 1) / 2);
				if (t < 0)t = 0;
				if (t > 1)t = 1;
			}
			else if (light->type() == typeid(DirectionalLight))
			{
				plh = -nl; // Normal from center of light to the intersection point ph
				nlh = glm::normalize(plh);
				cos0 = (glm::dot(nh, nlh) + 1) / 2; // Calculate cos 0 = (nh.nl + 1) / 2 for amount of diffuse
				t = cos0;
			}
			else if (light->type() == typeid(LineLight))
			{
				plh = glm::dot(ph - pl, nl) * nl + pl; // Vector from pl(point on line light) to ph
				t = (glm::dot(glm::normalize(plh - ph), nh) + 1) / 2;
			}

			if (material.hasTexture)
			{
				glm::vec2 uv = currentObj->findUV(ph, material.textureMap);
				c = material.textureMap.mapTexture(uv.x, uv.y);
			}
			else
				c = material.c2 *pow(t, 2) + material.c1 * 2 * (1 - t) * t + c * pow((1 - t), 2); // Three colors

			float tmin = INT_MAX;
			int ali = 0;
			float sh = 0.5f;
			Object *shadowObject = NULL;
			float d = glm::length(plh);
			bool inShadow = false;
			for each(Object* object in objects)
			{
				if (object != currentObj)
				{
					float ti = -1;
					if (light->type() == typeid(DirectionalLight))
					{
						// For directional light, we shoot a ray from ph to the object in the light direction and if it hits, 
						// then the pixel is a shadow
						ti = object->intersect(ph, nlh, index);
						if (0 < ti && ti < INT_MAX) // Current object pixel is in shadow
						{
							ali++;
							shadowObject = object;
							if (tmin > ti) {
								tmin = ti;
							}
							inShadow = true;
						}
					}
					else
					{
						ti = object->intersect(pl, -nlh, index);
						if (0 < ti && ti < d) // Current object pixel is in shadow
						{
							ali++;
							shadowObject = object;
							if (tmin > ti) {
								tmin = ti;
							}
							inShadow = true;
						}
					}
				}
			}
			// SHADOW
			if (inShadow)
			{
				sh = (tmin / d) * (2 * ali) * sh;
				if (sh < 0)sh = 0;
				if (sh > 1)sh = 1;
				//c = material.csh * sh + c * (1 - sh); // Three colors
			}
			else
			{
				if (material.isBorder) {
					// BORDER
					// Border b = 1 - neh.nh
					glm::vec3 neh = glm::normalize(cam.pe - ph);
					float b = 1 - glm::dot(neh, nh);
					b = (b - 0.7) / 0.9;
					if (b > 1) b = 1;
					if (b < 0) b = 0;
					//c = c * (1 - b) + material.cb * b;
				}

				if (material.isSpecular) {
					// SPECULAR
					// Reflection r = -v + 2 (v.n) n; v - normal vector from ph to camera eye nphe ; n - normal at intersection point nh
					glm::vec3 nphe = glm::normalize(cam.pe - ph);
					glm::vec3 reh = nphe - 2.0f * (glm::dot(nh, nphe) * nh);

					// Ellipsoid: 1 - pow(x / 2.0f, 2) - pow(y / 2.0f, 2)
					// Rectangular: 1 - max(abs(x) / 3.0f, abs(y) / 2.0f) 
					// Hyperbola: 1 + pow(x / 1.0f, 2) - pow(y / 0.5f, 2)
					n2 = glm::normalize(plh);
					n1 = glm::cross(n2, n0);
					float x = glm::dot(-plh, n0 - (n2 * glm::dot(reh, n0) / glm::dot(reh, n2)));
					float y = glm::dot(-plh, n1 - (n2 * glm::dot(reh, n1) / glm::dot(reh, n2)));
					//float s = 1 - pow(x / 2.0f, 2) - pow(y / 2.0f, 2);
					//if (s > 0.0f) {
					// Calculate cos fi for amount of specularity -nlh - Normal vector from ph to pl; re - reflection of camera eye at ph
					float cosfi = glm::dot(-nlh, reh);
					float s = (cosfi - 0.9) / 0.1;
					if (s > 1) s = 1;
					if (s < 0) s = 0;
					//c = c * (1 - s) + material.cs * s;
				}
			}
		}
		if (material.isReflective) {
			// REFLECTION
			float b = glm::dot(nh, -cam.npe);
			glm::vec3 reflection = glm::normalize(cam.npe - 2 * glm::dot(cam.npe, nh) * nh);

			// GLOSSY REFLECTION
			if (material.isReflective)// TODO: change it to isGlossy
			{
				//Glossy value
				float glossiness = 50.0;
				float rand0 = rand() % (int)glossiness;
				float rand1 = rand() % (int)glossiness;
				float rand2 = rand() % (int)glossiness;
				glm::vec3 glossyVector((rand0 - glossiness / 2) / PI, (rand1 - glossiness / 2) / PI, (rand2 - glossiness / 2) / PI);
				//reflection = reflection + glossyVector;

			}
			// ph hit point is new camera eye and reflection is direction
			return raytrace(ph, reflection, bounce + 1, ior);
		}
		if (material.isRefractive) {
			float phi = ior;
			glm::vec3 refraction;
			refraction = dir * (1 - phi) + nh * phi;
			// TRANSLUCENCY REFRACTION
			if (material.isRefractive)// TODO: change it to isTransparent
			{
				//Transluceny value
				float transparency = 10;
				float rand0 = rand() % (int)transparency;
				float rand1 = rand() % (int)transparency;
				float rand2 = rand() % (int)transparency;
				glm::vec3 translucentVector((rand0 - transparency/2) / PI, (rand1 - transparency/2) /PI, (rand2 - transparency/2) / PI);
				//refraction = refraction + translucentVector;

			}
			// ph hit point is new camera eye and refraction is direction
			return  raytrace(ph, refraction, bounce + 1, ior);
		}
		else
			return c;
	}

	Color antialias(glm::vec3 position, glm::vec3 direction, int i, int j, int bounce)
	{
		Color c(0, 0, 0);
		glm::vec3 aliasColor(0, 0, 0);
		float r0 = ((double)rand() / (RAND_MAX));
		float r1 = ((double)rand() / (RAND_MAX));
		// Loop for Antialiasing
		for (int m = 0; m < M; m++)
		{
			for (int n = 0; n < N; n++)
			{
				float x = i + (float)(m + r0) / M;
				float y = j + (float)(n + r1) / N;
				aliasColor = raytrace(position, direction, 1, 1.33f) + aliasColor;
			}
		}
		// Average value for the pixel
		c = aliasColor / (1.0f * N * M);
		return c;
	}

	void render(int frame, glm::vec3 sPos, float radius, float ior)
	{
		int bounce = 0;
		Color painterMap;
		camPaint = ior;// 0.001f;
		glm::vec3 painterCam;
		int size = 100 + (frame * 3);
		for (unsigned long int i = 0; i < WIDTH; i++)
		{
			for (unsigned long int j = 0; j < HEIGHT; j++)
			{
				
				int pixel = (j * WIDTH + i);
				pixmap.colors[pixel] = glm::vec3(0, 0, 0);
				float offset = 0.5;
				if (!DOF) {
					float x00 = (float)i / cam.xMax;
					float y00 = (float)j / cam.yMax;
					
					cam.pp = cam.p00 + x00 * cam.sx * cam.n0 + y00 * cam.sy * cam.n1;
					cam.npe = normalize(cam.pp - cam.pe);
				}
				spheres->pi = sPos;
				spheres->r = radius;

				if (CAMPAINT)
				{
					painterMap = camPaintMap->textureMap1.mapTexture(i%camPaintMap->textureMap1.width, j%camPaintMap->textureMap1.height);
					painterCam = glm::vec3(painterMap.r / size, painterMap.g / size, painterMap.b / size);
					// Camera painting
					glm::vec3 paint = glm::vec3(cam.pe.x + (painterCam.x * camPaint), cam.pe.y + (painterCam.y * camPaint), cam.pe.z + (painterCam.z * camPaint));
					cam.npe = normalize(cam.pp - paint);
				}
				glm::vec3 aliasColor(0, 0, 0);
				// Loop for Antialiasing
				for (int m = 0; m < M; m++)
				{
					for (int n = 0; n < N; n++)
					{
						//camPaint = camPaint + 50.0;
						if (DOF) {
							cam.pe = glm::vec3(0, 0, -30);
							float rx = (double)rand() / (double)RAND_MAX;
							float ry = (double)rand() / (double)RAND_MAX;

							float x0 = (i + (m / M) + (rx / M)) / cam.xMax;
							float y0 = (j + (n / N) + (ry / N)) / cam.yMax;

							rx = (ry * 2.0 - 1.0) * offset;
							ry = (ry * 2.0 - 1.0) * offset;
							float rz = ((double)rand() / (double)RAND_MAX * 2.0 - 1.0) * offset;
							//cam.pe = glm::vec3(cam.pe.x + rx, cam.pe.y + ry, cam.pe.z + rz);

							cam.pp = cam.p00 + x0 * cam.sx * cam.n0 + y0 * cam.sy * cam.n1;
							cam.npe = normalize(cam.pp - cam.pe);
						}
						//Motion blur
						if(MOTIONBLUR)
							spheres->pi = glm::vec3(spheres->pi.x + cam.n0.x * (0.1 * m), spheres->pi.y + cam.n0.y * (0.1 * n), spheres->pi.z);

						aliasColor = raytrace(cam.pe, cam.npe, 3, ior) + aliasColor;
					}
				}
				// Average value for the pixel
				pixmap.colors[pixel] = aliasColor / (1.0f * N * M);
			}
		}
		string frameName = "./proj";
		frameName.append(to_string(frame));
		frameName.append(".ppm");
		ofstream ofs(frameName.c_str(), std::ios::out | std::ios::binary);
		ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
		for (unsigned int i = 0; i < WIDTH * HEIGHT; ++i) {
			ofs << (unsigned char)(min(float(255), (float)pixmap.colors[i].r)) <<
				(unsigned char)(min(float(255), (float)pixmap.colors[i].g)) <<
				(unsigned char)(min(float(255), (float)pixmap.colors[i].b));
		}
		ofs.close();
	}
};
