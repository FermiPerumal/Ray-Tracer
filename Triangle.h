/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Triangle class derived from Object

***************************************/

class Triangle : public Object
{
public:
	glm::vec3 ni, p0, p1, p2; // Normal, vertex points on triangle
	glm::vec2 ti; // Texture uvs on triangle
	glm::vec3 v1, v2; // Vectors v1 and v2

	Triangle() {
	}

	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Material m)
		: p0(p0), p1(p1), p2(p2), Object(m) {
		v1 = p1 - p0;
		v2 = p2 - p0;
		ni = glm::normalize(glm::cross(v1, v2));
	}

	Triangle(obj_vector* ov0, obj_vector* ov1, obj_vector* ov2, obj_vector* n, obj_vector* t, Material m)
		: Object(m) {
		p0 = glm::vec3(ov0->e[0], ov0->e[1], ov0->e[2]);
		p1 = glm::vec3(ov1->e[0], ov1->e[1], ov1->e[2]);
		p2 = glm::vec3(ov2->e[0], ov2->e[1], ov2->e[2]);
		v1 = p1 - p0;
		v2 = p2 - p0;
		ni = glm::vec3(n->e[0], n->e[1], n->e[2]);
		//ti = glm::vec2(t->e[0], t->e[1]);
		//ni = glm::normalize(glm::cross(v1, v2));
	}

	glm::vec3 normal(glm::vec3 &p, int &index) const
	{
		return ni;
	}

	glm::vec3 point() const
	{
		return p0;
	}

	float size() const
	{
		return -1.0f;
	}

	float intersect(glm::vec3 position, glm::vec3 direction, int &index) const
	{
		float t = INT_MAX;
		if (glm::dot(ni, position - p0) >= 0)
		{
			// Camera eye position is not on the other side of the plane
			if (glm::dot(ni, direction) < 0)
			{
				//float d = glm::dot(ni, p0);
				//float v = glm::dot(ni, ni * d - position);
				// Distance from camera
				t = -glm::dot(ni, position - p0) / glm::dot(ni, direction); // v / glm::dot(ni, direction);
				// Find intersection point on triangle
				glm::vec3 ph = direction * t + position;

				glm::vec3 a = glm::cross(p2 - p0, p1 - p0);
				glm::vec3 a0 = glm::cross(p1 - ph, p2 - ph);
				glm::vec3 a1 = glm::cross(p2 - ph, p0 - ph);
				glm::vec3 a2 = glm::cross(p0 - ph, p1 - ph);
				bool isIntersect = false;
				if (a.x > a.y && a.x > a.z && a.x != 0.0)
				{
					if (a0.x / a.x > 0.0 && a1.x / a.x > 0.0 && a2.x / a.x > 0.0)
						isIntersect = true;
					else if (a0.x / a.x < 0.0 && a1.x / a.x < 0.0 && a2.x / a.x < 0.0)
						isIntersect = true;
				}
				else if (a.y > a.x && a.y > a.z && a.y != 0.0)
				{
					if (a0.y / a.y > 0.0 && a1.y / a.y > 0.0 && a2.y / a.y > 0.0)
						isIntersect = true;
					else if (a0.y / a.y < 0.0 && a1.y / a.y < 0.0 && a2.y / a.y < 0.0)
						isIntersect = true;
				}
				else if (a.z != 0.0)
				{
					if (a0.z / a.z > 0.0 && a1.z / a.z > 0.0 && a2.z / a.z > 0.0)
						isIntersect = true;
					else if (a0.z / a.z < 0.0 && a1.z / a.z < 0.0 && a2.z / a.z < 0.0)
						isIntersect = true;
				}

				if (isIntersect)
					return t;
			}
		}
		return -1;
	}


	glm::vec2 findUV(glm::vec3 ph, ppmImage image) const
	{
		glm::vec3 phi = ph - glm::vec3(-256, -15, 512);
		float u = phi.x;
		float v = phi.y;
		//return ti;
		return glm::vec2(u, v);
	}
	
	Color getColor(glm::vec3 ph, Camera &cam, vector<Light*> &lights, vector<Object*>& objects, int &triIndex) const
	{
		Color c = material.c0;
		glm::vec3 nh = normal(ph, triIndex); // Normal at the intersection point ph from the center of the sphere
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
				glm::vec2 uv = findUV(ph, material.textureMap);
				float i = uv.x / material.textureMap.width;
				float j = uv.y / material.textureMap.height;
				c = material.textureMap.mapTexture(uv.x, uv.y);
			}
			else
				c = material.c2 *pow(t, 2) + material.c1 * 2 * (1 - t) * t + c * pow((1 - t), 2); // Three colors

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
						ti = object->intersect(ph, nlh, triIndex);
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
						ti = object->intersect(pl, -nlh, triIndex);
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
		return c;
	}
};
