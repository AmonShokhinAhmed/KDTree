#pragma once
#include <vector>
#include "Entity.h";
#include <sstream>
#include <iomanip>
#include "Datastructures.h"

//#define DEBUG_VISUALS
#define DEBUG_INFORMATION
enum VisualMode {GridPlane, BoundingBox, None};
enum InsertionMode {Vertices, Indices, Triangles};
class KDTree
{
public:
	KDTree(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, unsigned int approxChildTris, bool performanceMode,  VisualMode visualMode, InsertionMode insertionMode);
	~KDTree();
	Hit* RayIntersect(Ray ray);
private:
	Node* root;
	std::vector<Entity*> _lines;
	std::vector<SimpleTriangle*> _triangles;
	std::vector<Node*> _nodes;
#ifdef DEBUG_VISUALS
	std::vector<Entity*> _debugTriangles;
#endif // DEBUG_VISUALS

	void insertUsingVertices(std::vector<glm::vec3> vertices, std::vector<unsigned int>indices, int depth);
	void insertUsingIndices(std::vector<glm::vec3> vertices, std::vector<unsigned int>indices, int depth);
	void insertUsingTriangles(std::vector<glm::vec3> vertices, std::vector<unsigned int>indices, int depth);
	void insertTrianglesAsNode(Node*& node, std::vector<SimpleTriangle*> triangles, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth);
	void insertVerticesAsNode(Node*& node,  std::vector<glm::vec3> vertices, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth);
	void insertTriangleAsLeaf(Node* node, SimpleTriangle*& triangle);

	void printNodeStatistic();

	void visitNodes(Node* node, Ray ray, Hit*& hit);
	void intersectNode(Node* node, Ray ray, Hit*& hit);
	float intersectTriangle(SimpleTriangle*& triangle, Ray ray);


	//generate lines visualising the KDTree
	void generateLines(VisualMode visualMode);
	//drawing a AABB
	void drawAABB(glm::vec3 min, glm::vec3 max, glm::vec4 color);
	//drawing planes exactly at the splitting planes
	void drawRecursiveGridPlanes(Node* node, glm::vec3 min, glm::vec3 max);
	//drawing bounding boxes for geometry under the node
	void drawRecursiveBoundingBoxes(Node* node);

	//finding the lowest j value of vertices under node
	glm::vec3 findMinValueUnderNode(Node* node, unsigned int j);
	//finds the lowest j value of geometry under node
	SimpleTriangle* findMinChildTriangle(Node* node, unsigned int j);
	//finds the minimum triangle under node (using a custom comparator)
	glm::vec3 findMinChildPoint(Node* node, unsigned int j);

	//finding the highest j value of vertices under node
	glm::vec3 findMaxPointFromNode(Node* node, unsigned int j);
	//finds the highest j value of geometry under node
	SimpleTriangle* findMaxChildTriangle(Node* node, unsigned int j);
	//finds the maximum triangle under node (using a custom comparator)
	glm::vec3 findMaxChildPoint(Node* node, unsigned int j);
};

