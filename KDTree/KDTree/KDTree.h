#pragma once
#include <vector>
#include "Entity.h";

struct Node {
	glm::vec3 vertex;
	unsigned int j;
	Node* left;
	Node* right;
};
class KDTree
{
public:
	KDTree(std::vector<glm::vec3> vertices, bool visual = true);
	glm::vec3 FindMin(unsigned int j);
	glm::vec3 FindMax(unsigned int j);
	~KDTree();
private:
	Node* insertNode(Node*& root, glm::vec3 vertex, unsigned int j);
	void insertVertices( std::vector<glm::vec3> vertices, unsigned int start, unsigned int end);
	int compareVertices(glm::vec3 a, glm::vec3 b, unsigned int j);
	void buildTree(std::vector<glm::vec3 >vertices);
	void generateLines();
	void drawLinesForAABB(glm::vec3 min, glm::vec3 max, glm::vec4 color);
	void drawGridPlanesForNode(Node* node, glm::vec3 min, glm::vec3 max, unsigned int depth, unsigned int maxDepth);
	void drawBoundingBoxForNode(Node* node,unsigned int depth, unsigned int maxDepth);
	std::vector<Entity*> _lines;
	Node* root;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 findMinFromNode(Node* node, unsigned int j);
	glm::vec3 findMaxFromNode(Node* node, unsigned int j);
};

