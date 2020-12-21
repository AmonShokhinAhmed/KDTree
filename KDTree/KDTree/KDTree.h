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
	~KDTree();
private:
	Node* insertNode(Node* root, glm::vec3 vertex, unsigned int j);
	int compareVertices(glm::vec3 a, glm::vec3 b, unsigned int j);
	void buildTree(std::vector<glm::vec3 >vertices);
	void generateLines();
	void drawLinesForAABB(glm::vec3 min, glm::vec3 max);
	void drawLinesForNode(Node* node, glm::vec3 min, glm::vec3 max, unsigned int depth, unsigned int maxDepth);
	std::vector<Entity*> _lines;
	Node* root;
	glm::vec3 min;
	glm::vec3 max;
};

