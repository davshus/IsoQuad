#pragma once
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
using namespace std;
using namespace glm;
class axes {
	public:
		vec3 up, down, left, right, forward, back;
		void calculate(quat);
};
void axes::calculate(quat q) {
	this->up = q * vec3(0, 1, 0);
	this->down = q * vec3(0, -1, 0);
	this->right = q * vec3(1, 0, 0);
	this->left = q * vec3(-1, 0, 0);
	this->back = q * vec3(0, 0, 1);
	this->forward = q * vec3(0, 0, -1);
}