/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Header Light class

***************************************/

#include "glm\glm\glm.hpp"
#include <typeinfo>
class Light
{
public:
	glm::vec3 nl; // Direction of the light
	glm::vec3 pl; // Position of the light
	Light() {

	}

	Light(glm::vec3 &nl, glm::vec3 &pl)
		: nl(nl), pl(pl) {
	}

	virtual glm::vec3 direction() const = 0;
	virtual glm::vec3 position() const = 0;

	const type_info& type() {
		return typeid(*this);
	};

	virtual ~Light() {};
};

class PointLight : public Light
{
public:
	PointLight() {
	}

	PointLight(glm::vec3 &nl, glm::vec3 &pl) :
		Light(nl, pl) { // Need both direction and position of light
	}

	glm::vec3 direction() const
	{
		return glm::normalize(nl);
	}

	glm::vec3 position() const
	{
		return pl;
	}
};

class SpotLight : public Light
{
public:
	SpotLight() {
	}

	SpotLight(glm::vec3 &nl, glm::vec3 &pl) :
		Light(nl, pl) { // Need both direction and position of light
	}

	glm::vec3 direction() const
	{
		return glm::normalize(nl);
	}

	glm::vec3 position() const
	{
		return pl;
	}
};

class DirectionalLight : public Light
{
public:
	DirectionalLight() {
	}

	DirectionalLight(glm::vec3 &nl, glm::vec3 &pl = glm::vec3(0, 0, 0)) :
		Light(nl, pl) { // Need just the direction of light
	}

	glm::vec3 direction() const
	{
		return glm::normalize(nl);
	}

	glm::vec3 position() const
	{
		return pl;
	}
};

class LineLight : public Light
{
public:
	LineLight() {
	}

	LineLight(glm::vec3 &nl, glm::vec3 &pl = glm::vec3(0, 0, 0)) :
		Light(nl, pl) { // Need both direction and a point on the light
	}

	glm::vec3 direction() const
	{
		return glm::normalize(nl);
	}

	glm::vec3 position() const
	{
		return pl;
	}
};
