#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera.hpp"

namespace gps {

	//Return the value of P(t), where t is in [0,1]
	glm::vec3 getParametricPoint(float t, glm::vec3 p0, glm::vec3 p1);


	//Paint the pixels that are on the line P0P1
	void drawParametricLinePoints(glm::vec3 p0, glm::vec3 p1);


	//Return the value of B(t), where t is in [0,1]. The value of B(t) is computed by taking into account all the 
	//controll points contained within the input vector
	glm::vec3 getBezierPoint(std::vector<glm::vec3> controlPoints, float t, int length);


	//Paint the pixels that are on the Bezier curve defined by the given control points
	void drawBezierPoints(std::vector<glm::vec3> controlPoints, gps::Camera &camera);

}