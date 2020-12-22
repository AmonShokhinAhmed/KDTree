#pragma once
#include <vector>
#include "Entity.h";
#include <sstream>
#include <iomanip>

struct Node {
	glm::vec3 vertex;
	unsigned int j;
	Node* left;
	Node* right;
	std::vector<glm::vec3>* childPoints;
}; 
struct Vec3Comparator {
	Vec3Comparator(unsigned int j) :J(j) { }
	bool operator () (glm::vec3 a, glm::vec3 b) {
		switch (J) {
		case 0:
			return (a.x < b.x);
			break;
		case 1:
			return (a.y < b.y);
			break;
		default:
			return (a.z < b.z);
			break;
		}
	}

	int J;
};
static string vec3ToString(glm::vec3 v)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << v.x << "|" << v.y << "|" << v.z;
	return stream.str();
}

class KDTree
{
public:
	KDTree(std::vector<glm::vec3> vertices, unsigned int depth, bool visual = true);
	glm::vec3 FindMin(unsigned int j);
	glm::vec3 FindMax(unsigned int j);
	~KDTree();
private:
	Node* root;
	std::vector<Entity*> _lines;

	glm::vec3 findMinFromNode(Node* node, unsigned int j);
	glm::vec3 findMinChildPoint(Node* node, unsigned int j);
	glm::vec3 findMaxFromNode(Node* node, unsigned int j);
	glm::vec3 findMaxChildPoint(Node* node, unsigned int j);
	void insertPoint(Node* node, glm::vec3& point);

	void insertVerticesAsNode(std::vector<glm::vec3>& vertices, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth);
	void insertNode(Node*& root, glm::vec3 vertex, unsigned int j);

	void generateLines();
	void drawLinesForAABB(glm::vec3 min, glm::vec3 max, glm::vec4 color);
	void drawGridPlanesForNode(Node* node, glm::vec3 min, glm::vec3 max);
	void drawBoundingBoxForNode(Node* node);
	
};

