/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Parent Object header class

***************************************/

#include "glm\glm\glm.hpp"
#include "Material.h"
#include "Light.h"

using std::vector;

class Object
{
public:
	Material material;
	virtual glm::vec3 normal(glm::vec3 &p, int &index) const = 0;
	virtual glm::vec3 point() const = 0;
	virtual float size() const = 0;
	virtual float intersect(glm::vec3 position, glm::vec3 direction, int &index) const = 0;
	virtual glm::vec2 findUV(glm::vec3 ph, ppmImage image) const = 0;
	virtual Color getColor(glm::vec3 ph, Camera &cam, vector<Light*>& lights, vector<Object*>& objects, int &index) const = 0;
	Object() {

	}
	
	Object(Material &m)
	: material(m) {
	}

	virtual ~Object() {};
};
