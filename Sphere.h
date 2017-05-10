/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Sphere class derived from Object

***************************************/

#define max(a,b) ((a) > (b) ? (a) : (b))

class Sphere : public Object
{
public:
	glm::vec3 pi;
	float r, r2;
	Sphere() {
	}

	Sphere(glm::vec3 &c,  float r,  Material &m)
		: pi(c), r(r), Object(m) {
		r2 = r * r;
	}

	glm::vec3 normal(glm::vec3 &p, int &index) const
	{
		glm::vec3 nhi = (p - pi) / r;
		glm::vec3 n2(0, 1, 0);
		if (material.hasNormal) {
			glm::vec2 uv = findUV(p, material.normalMap);
			glm::vec3 v0 = glm::normalize(glm::cross(n2, nhi));
			glm::vec3 v1 = glm::normalize(glm::cross(nhi, v0));
			Color nColor = material.normalMap.mapTexture(uv.x, uv.y);
			glm::vec3 nc = glm::normalize(glm::vec3(nColor.r, nColor.g, nColor.b));
			nhi = float(nc.r) * v0 + float(nc.g) * v1 + float(nc.b) * nhi;
			return nhi;
		}
		else
			return  nhi;
	}

	glm::vec3 point() const
	{
		return pi;
	}

	float size() const
	{
		return r;
	}

	float intersect(glm::vec3 position, glm::vec3 direction, int &index) const
	{
		float t = INT_MAX;
		float ci = glm::dot(position - pi, position - pi) - (r * r);
		if (ci >= 0)
		{
			// Camera eye/light position is outside the sphere
			float bi = glm::dot(direction, pi - position);
			float delta = bi * bi - ci;
			if (delta >= 0 && bi >= 0)
			{
				//Intersection exists
				t = bi - sqrt(delta);
			}
		}
		return t;
	}

	glm::vec2 findUV(glm::vec3 p, ppmImage image) const
	{
		float piValue = 3.14;
		glm::vec3 phi = glm::normalize(p - pi);
		glm::vec3 pole0(0, 0, 1), pole1(-1, 0, 0), pole2(0, -1, 0);
		float x = glm::dot(pole0, phi);
		float y = glm::dot(pole1, phi);
		float z = glm::dot(pole2, phi);
		float v = (acos(z) / (piValue));
		float u0 = y / sqrt(1 - pow(z, 2));
		if (u0 < -1) u0 = -1;
		if (u0 > 1) u0 = 1;
		float u = acos(u0) / (2 * piValue);
		if (x < 0)
			u = 1 - u;
		return glm::vec2(u * image.width, v * image.height);
	}

	Color getSkyColor(glm::vec3 direction) const
	{
		Color c;
		if (material.hasTexture)
		{
			glm::vec2 uv = findUV(direction, material.textureMap1);
			c = material.textureMap1.mapTexture(uv.x, uv.y);
		}
		return c;
	}

	Color getColor(glm::vec3 ph, Camera &cam, vector<Light*>& lights, vector<Object*>& objects, int &index) const
	{
		Color c;
		glm::vec3 nh = normal(ph, index); // Normal at the intersection point ph from the center of the sphere
		float nhh = glm::length(nh);
		glm::vec3 pl(0, 0, 0), nl(0, 0, 0);
		glm::vec3 nlh(0, 0, 0), plh(0, 0, 0);
		glm::vec3 n2(0, 0, 0), n0(1, 0, 0), n1(0, 0, 0);
		float cos0, t;
		
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
				t = ((cos01 + 1) / 2) * ((cos02 + 1) / 2) * 1.5f;
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
				glm::vec2 uv = findUV(ph, material.textureMap);
				c = material.textureMap1.mapTexture(uv.x, uv.y) * t + material.textureMap.mapTexture(uv.x, uv.y) * (1 - t);
			}
			else
				c = material.c2 *pow(t, 2) + material.c1 * 2 * (1 - t) * t + material.c0 * pow((1 - t), 2); // Three colors

			// SHADOWS
			float tmin = INT_MAX;
			int ali = 0;
			float sh = 0.5f;
			Object *shadowObject = NULL;
			float d = glm::length(plh);
			bool inShadow = false;
			for each(Object* object in objects)
			{
				if (object != this)
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
			
			if (inShadow)
			{
				sh = (tmin / d) * (2 * ali) * sh; // As the number of objects hide, the shadow becomes darker using 1/ali term
				if (sh < 0)sh = 0;
				if (sh > 1)sh = 1;
				c = material.csh * sh + c * (1 - sh); // Three colors
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
					c = c * (1 - b) + material.cb * b;
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
					c = c * (1 - s) + material.cs * s;
				}
			}
		}
		return c;
	}
};
