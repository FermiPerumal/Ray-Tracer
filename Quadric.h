/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Quadric class derived from Object

***************************************/

class Quadric : public Object
{
public:
	short int a[3], a21, a00; // Indicators for the quadric
	float s[3]; // Adjustment parameters
	glm::vec3 pc; // Center of quadric
	Quadric() {
	}

	Quadric(short int a02, short int a12, short int a22, short int a21, short int a00, float s0, float s1, float s2, glm::vec3 &pc,  Material &m)
		:  a21(a21), a00(a00), pc(pc), Object(m) {
		a[0] = a02; a[1] = a12; a[2] = a22;
		s[0] = s0; s[1] = s1; s[2] = s2;
	}

	glm::vec3 normal(glm::vec3 &p, int &index) const
	{
		return p;
	}

	glm::vec3 point() const
	{
		return pc;
	}

	float size() const
	{
		return -1.0f;
	}

	bool isOutsideShape(Camera &cam) const
	{
		float fp = 0.0f;
		glm::vec3 n[3]; // Normals of camera stored as array for looped calculations
		n[0] = cam.n0; n[1] = cam.n1; n[2] = cam.n2;
		for (int i = 0; i < 3; i++)
		{
			fp += a[i] * pow(glm::dot(n[i], cam.pe - pc) / s[i], 2);
		}
		fp += a21 * glm::dot(n[2], cam.pe - pc) / s[2];
		fp += a00;
		if (fp < 0)
			return false;
		return true;
	}

	float intersect(glm::vec3 position, glm::vec3 direction, int &index) const
	{
		return -1;
	}

	float intersect(Camera &cam) const
	{
		float t1 = INT_MAX, t2 = INT_MAX;
		glm::vec3 n[3]; // Normals of camera stored as array for looped calculations
		n[0] = cam.n0; n[1] = cam.n1; n[2] = cam.n2;
		if (isOutsideShape(cam))
		{
			// Camera eye position is outside the quadric
			float ai = 0.0f, bi = 0.0f, ci = 0.0f;
			for (int i = 0; i < 3; i++)
			{
				ai += a[i] * pow(glm::dot(n[i], cam.npe) / s[i], 2);
				bi += a[i] * (2 * glm::dot(n[i], cam.npe) * glm::dot(n[i], cam.pe - pc)) / pow(s[i], 2);
				ci += a[i] * pow(glm::dot(n[i], cam.pe - pc) / s[i], 2);
			}
			bi += a21 * glm::dot(n[2], cam.npe) / s[2];
			ci += (a21 * glm::dot(n[2], cam.pe - pc) / s[2]) + a00;
			float delta = bi * bi - 4 * ai * ci;
			if (delta >= 0)
			{
				//Intersection exists
				t1 = (-bi - sqrt(delta)) / (2 * ai);
				t2 = (-bi + sqrt(delta)) / (2 * ai);
			}
		}
		if (t1 < 0) 
			t1 = INT_MAX;
		if (t2 < 0) 
			t2 = INT_MAX;
		// Return the closest t value
		return t1;
	}

	Color getColor(glm::vec3 ph, Camera &cam , vector<Light*> &lights, vector<Object*>& objects) const
	{
		return DARKLILAC;
	}
};
