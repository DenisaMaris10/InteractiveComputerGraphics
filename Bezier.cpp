#include "Bezier.h"
#include "math.h"
#include <vector>
#include "Camera.hpp"


namespace gps {
	//Return the value of P(t), where t is in [0,1]
	glm::vec3 getParametricPoint(float t, glm::vec3 p0, glm::vec3 p1) {
		//P(t) = (1 - t)*P0 + t*P1
		glm::vec3 point;
		point.x = (1 - t) * p0.x + t * p1.x;
		point.y = (1 - t) * p0.y + t * p1.y;
		point.z = (1 - t) * p0.z + t * p1.z;

		return point;
	}

	//Paint the pixels that are on the line P0P1
	void drawParametricLinePoints(glm::vec3 p0, glm::vec3 p1) {
		//Hint: To paint a single pixel, you can use the function: SDL_RenderDrawPoint(renderer, x, y)
		glm::vec3 p;
		for (float t = 0; t <= 1; t = t + 0.005)
		{
			p = getParametricPoint(t, p0, p1);
		}
	}

	//Return the value of B(t), where t is in [0,1]. The value of B(t) is computed by taking into account all the 
	//controll points contained within the input vector
	glm::vec3 getBezierPoint(std::vector<glm::vec3> controlPoints, float t, int length) {
		glm::vec3 point;
		if (controlPoints.size() == 2)
			return getParametricPoint(t, controlPoints.at(0), controlPoints.at(1));

		std::vector<glm::vec3> controlPoints1;
		std::vector<glm::vec3> controlPoints2;
		controlPoints1.push_back(controlPoints.at(0));
		for (int i = 1; i < length - 1; i++)
		{
			controlPoints1.push_back(controlPoints.at(i));
			controlPoints2.push_back(controlPoints.at(i));
		}
		controlPoints2.push_back(controlPoints.at(length - 1));
		return getBezierPoint(controlPoints1, t, length - 1) * (1 - t) + getBezierPoint(controlPoints2, t, length - 1) * t;
	}

	//Paint the pixels that are on the Bezier curve defined by the given control points
	void drawBezierPoints(std::vector<glm::vec3> controlPoints, gps::Camera &camera) {

		glm::vec3 point;
		for (float t = 0; t <= 1; t += 0.05)
		{
			point = getBezierPoint(controlPoints, t, controlPoints.size());
			camera.setCameraPosition(point);
			camera.setCameraTarget(camera.getCameraPosition() + camera.getCameraFrontDirection());
			printf("Sunt aici si calculez %f %f %f %f\n", point.x, point.y, camera.getCameraPosition().x, camera.getCameraPosition().y);
		}
	}
}