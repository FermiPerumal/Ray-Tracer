/***************************************

Ray Tracer

:author:
	Fermi Perumal

:description:
	Header class for a camera

***************************************/

#include "glm\glm\glm.hpp"

class Camera
{
public:
	glm::vec3 vView, vUp, v0; // vView - camera view direction; vUp - camera up direction; v0 = vView X vUp
	glm::vec3 n2, n0, n1, npe; // n2 - normalized vView; n0 - normalized v0; n1 = n0 X n2; npe - normalized (pp - pe)
	glm::vec3 pe, pc, p00, pp; // pe - eye point; pc = center of view-port; p00 - lower left of view-port; pp - position in view-port
	int xMax, yMax; // Max and Min of image size; xMax - width; yMax - height
	float sx, sy; // Scaling of x and y
	float d; // Distance between eye and camera

	Camera() {
	}

	Camera(glm::vec3 &view, glm::vec3 &up, glm::vec3 &eye, float dist, int width, int height, float s)
		: vView(view), vUp(up), pe(eye), d(dist), xMax(width), yMax(height), sx(s)
	{
		v0 = glm::cross(vView, vUp);
		n2 = glm::normalize(vView);
		n0 = glm::normalize(v0);
		n1 = glm::cross(n2, n0);
		pc = pe + d * n2;
		sy = sx * yMax / xMax;
		p00 = pc - n0 *sx / 2.0f - n1 * sy / 2.0f;
	}
};
