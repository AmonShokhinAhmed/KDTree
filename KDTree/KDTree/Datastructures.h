#pragma once
#include <glm/glm.hpp>
#include <vector>


struct SimpleTriangle {
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	bool checked;
#ifdef DEBUG_VISUALS
	Entity* debugTriangle;
#endif // DEBUG_VISUALS
};
struct Ray {
	glm::vec3 A;
	glm::vec3 D;
	float TMax;
};
struct Hit {
	SimpleTriangle* Triangle;
	glm::vec3 point;
};
struct Node {
	Node(float value, unsigned int j) :
		Value(value),J(j)
	{
		Children[0] = nullptr;
		Children[1] = nullptr;
		ChildTriangles = nullptr;
	}
	float Value;
	unsigned int J;
	Node* Children[2];
	std::vector<SimpleTriangle*>* ChildTriangles;
};