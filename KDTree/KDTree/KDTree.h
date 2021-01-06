#pragma once
#include <vector>
#include "Entity.h";
#include <sstream>
#include <iomanip>




struct SimpleTriangle {
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	bool checked;
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
	glm::vec3 vertex;
	unsigned int j;
	Node* left;
	Node* right;
	std::vector<SimpleTriangle*>* childTriangles;
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
struct TriangleComparator {
	TriangleComparator(unsigned int j) :J(j) { }
	bool operator () (SimpleTriangle* triA, SimpleTriangle* triB) {
		float aValue;
		float bValue;
		switch (J) {
		case 0:
			aValue = triA->a.x < triA->b.x ? (triA->a.x < triA->c.x ? triA->a.x : triA->c.x) : (triA->b.x < triA->c.x ? triA->b.x : triA->c.x);
			bValue = triB->a.x < triB->b.x ? (triB->a.x < triB->c.x ? triB->a.x : triB->c.x) : (triB->b.x < triB->c.x ? triB->b.x : triB->c.x);
			return (aValue < bValue);
			break;
		case 1:
			aValue = triA->a.y < triA->b.y ? (triA->a.y < triA->c.y ? triA->a.y : triA->c.y) : (triA->b.y < triA->c.y ? triA->b.y : triA->c.y);
			bValue = triB->a.y < triB->b.y ? (triB->a.y < triB->c.y ? triB->a.y : triB->c.y) : (triB->b.y < triB->c.y ? triB->b.y : triB->c.y);
			return (aValue < bValue);
			break;
		default:
			aValue = triA->a.z < triA->b.z ? (triA->a.z < triA->c.z ? triA->a.z : triA->c.z) : (triA->b.z < triA->c.z ? triA->b.z : triA->c.z);
			bValue = triB->a.z < triB->b.z ? (triB->a.z < triB->c.z ? triB->a.z : triB->c.z) : (triB->b.z < triB->c.z ? triB->b.z : triB->c.z);
			return (aValue < bValue);;
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
	KDTree(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, unsigned int approxChildTris, bool visual = true);
	glm::vec3 FindMinPoint(unsigned int j);
	glm::vec3 FindMaxPoint(unsigned int j); 
	SimpleTriangle* FindMinTriangle(unsigned int j);
	//SimpleTriangle FindMaxTriangle(unsigned int j);
	Hit* RayIntersect(Ray ray);
	~KDTree();
private:
	Node* root;
	std::vector<Entity*> _lines;
	std::vector<SimpleTriangle*> _triangles;
	SimpleTriangle* findMinTriangleFromNode(Node* node, unsigned int j);
	glm::vec3 findMinPointFromNode(Node* node, unsigned int j);
	SimpleTriangle* findMinChildTriangle(Node* node, unsigned int j);
	glm::vec3 findMinChildPoint(Node* node, unsigned int j);

	//SimpleTriangle findMaxTriangleFromNode(Node* node, unsigned int j);
	glm::vec3 findMaxPointFromNode(Node* node, unsigned int j);
	//SimpleTriangle findMaxChildTriangle(Node* node, unsigned int j);
	glm::vec3 findMaxChildPoint(Node* node, unsigned int j);

	//void insertPoint(Node* node, glm::vec3& point);
	void insertVerticesAsNode(std::vector<glm::vec3> vertices, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth);
	void insertNode(Node*& root, glm::vec3 vertex, unsigned int j);
	void insertTriangle(Node* node, SimpleTriangle*& triangle);

	void visitNodes(Node* node, Ray ray, Hit*& hit);
	void intersectNode(Node* node, Ray ray, Hit*& hit);
	float intersectTriangle(SimpleTriangle*& triangle, Ray ray);


	void generateLines();
	void drawLinesForAABB(glm::vec3 min, glm::vec3 max, glm::vec4 color);
	void drawGridPlanesForNode(Node* node, glm::vec3 min, glm::vec3 max);
	void drawBoundingBoxForNode(Node* node);
	
};

