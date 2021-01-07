#include "KDTree.h"
#include "Utils.h"
#include <chrono>

#pragma region Constructor/Destructor

KDTree::KDTree(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, unsigned int approxChildTris, bool performanceMode , VisualMode visualMode,InsertionMode insertionMode)
{
#ifdef DEBUG_INFORMATION
	auto start = std::chrono::high_resolution_clock::now();
#endif // DEBUG_INFORMATION
	if (vertices.size() <= 0 || indices.size()<=0) {
		return;
	}

	//calculating dynamic depth based on an approximation of max tris under the leaf nodes
	int triAmount = indices.size() / 3;
	float leafNodeAmount =  (float)triAmount / (float)approxChildTris; 
	float depth = glm::log2(leafNodeAmount);
	depth = glm::ceil(depth);
	depth = glm::max(0.0f, depth);
	depth += 1;

	//inserting everything into the tree, thus filling the leafnodes
	switch (insertionMode) {
	case InsertionMode::Vertices:
		insertUsingVertices(vertices, indices, depth);
		break;
	case InsertionMode::Indices:
		insertUsingIndices(vertices, indices, depth);
		break;
	case InsertionMode::Triangles:
		insertUsingTriangles(vertices, indices, depth);
		break;
	}

	//generating visualisation of KDTree
	if (visualMode!=VisualMode::None) {
		generateLines(visualMode);
	}

#ifdef DEBUG_INFORMATION
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "KDTree building took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl << std::endl;
	std::cout << "Depth: " << depth << std::endl;
	printNodeStatistic();
#endif // DEBUG_INFORMATION
}

//Destructor, deleting all visualisation lines, triangles, and nodes
KDTree::~KDTree()
{
	for (auto e : _lines) {
		delete e;
	}
	_lines.clear();
	root = nullptr;
	for (auto n : _nodes) {
		delete n->ChildTriangles;
		delete  n;
	}
	_nodes.clear();
	for (auto t : _triangles) {
		delete t;
	}
	_triangles.clear();
}

#pragma endregion

#pragma region Tree Building

//inserting into the tree using only the vertices (so no duplicates are used)
void KDTree::insertUsingVertices(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, int depth)
{
	//inserting all vertices to create non leaf nodes
	insertVerticesAsNode(root, vertices, 0, vertices.size(), 0, depth);
	
	//creating all triangles and inserting them into the tree, thus filling the leaf nodes
	for (int i = 0; i < indices.size(); i += 3) {
		SimpleTriangle* triangle = new SimpleTriangle();
		triangle->a = vertices[indices[i]];
		triangle->b = vertices[indices[i + 1]];
		triangle->c = vertices[indices[i + 2]];
		triangle->checked = false;

#ifdef DEBUG_VISUALS
		auto debugTriangle = new Entity();
		debugTriangle->AddComponent<Triangle>();
		debugTriangle->GetComponent<Triangle>()->SetPoints(triangle->a, triangle->b, triangle->c);
		debugTriangle->GetComponent<Triangle>()->Color = glm::vec4(0, 1, 0, 1);
		debugTriangle->GetComponent<Triangle>()->Active = false;
		_debugTriangles.push_back(debugTriangle);
		triangle->debugTriangle = debugTriangle;
#endif //DEBUG_VISUALS
		_triangles.push_back(triangle);
		insertTriangleAsLeaf(root, triangle);
	}
}
//inserting into the tree using indices, so duplicate vertices are used
void KDTree::insertUsingIndices(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, int depth)
{
	//creating a vector using possible duplicate verices
	std::vector<glm::vec3>actualVertices;
	for (auto i : indices) {
		actualVertices.push_back(vertices[i]);
	}
	//inserting them into the tree, thus creating non leaf nodes
	insertVerticesAsNode(root, actualVertices, 0, actualVertices.size(), 0, depth);

	//creating all triangles and inserting them into the tree, thus filling the leaf nodes
	for (int i = 0; i < indices.size(); i += 3) {
		SimpleTriangle* triangle = new SimpleTriangle();
		triangle->a = vertices[indices[i]];
		triangle->b = vertices[indices[i + 1]];
		triangle->c = vertices[indices[i + 2]];
		triangle->checked = false;

#ifdef DEBUG_VISUALS
		auto debugTriangle = new Entity();
		debugTriangle->AddComponent<Triangle>();
		debugTriangle->GetComponent<Triangle>()->SetPoints(triangle->a, triangle->b, triangle->c);
		debugTriangle->GetComponent<Triangle>()->Color = glm::vec4(0, 1, 0, 1);
		debugTriangle->GetComponent<Triangle>()->Active = false;
		_debugTriangles.push_back(debugTriangle);
		triangle->debugTriangle = debugTriangle;
#endif //DEBUG_VISUALS
		_triangles.push_back(triangle);
		insertTriangleAsLeaf(root, triangle);
	}
}
//inserting into the tree using the actual triangles that will be intersected later
void KDTree::insertUsingTriangles(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, int depth)
{
	//creating all tris and storing them in a vector
	for (int i = 0; i < indices.size(); i += 3) {
		SimpleTriangle* triangle = new SimpleTriangle();
		triangle->a = vertices[indices[i]];
		triangle->b = vertices[indices[i + 1]];
		triangle->c = vertices[indices[i + 2]];
		triangle->checked = false;

#ifdef DEBUG_VISUALS
		auto debugTriangle = new Entity();
		debugTriangle->AddComponent<Triangle>();
		debugTriangle->GetComponent<Triangle>()->SetPoints(triangle->a, triangle->b, triangle->c);
		debugTriangle->GetComponent<Triangle>()->Color = glm::vec4(0, 1, 0, 1);
		debugTriangle->GetComponent<Triangle>()->Active = false;
		_debugTriangles.push_back(debugTriangle);
		triangle->debugTriangle = debugTriangle;
#endif //DEBUG_VISUALS
		_triangles.push_back(triangle);
	}
	//inserting that vector to create non leaf nodes
	insertTrianglesAsNode(root, _triangles, 0, _triangles.size(), 0, depth);


	//inserting the triangles to create leaf nodes
	for (auto t : _triangles) {
		insertTriangleAsLeaf(root, t);
	}
}

//create nodes using a vector of triangles
void KDTree::insertTrianglesAsNode(Node*& node, std::vector<SimpleTriangle*> triangles, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth) {
	//break if maxdepth is reached (or there are no more triangles)
	if (start >= end || depth >= maxDepth) {
		return;
	}

	//calculate extreme values on all axis
	glm::vec3 currentMin;
	glm::vec3 currentMax;
	for (int i = 0; i < 3; i++) {
		SimpleTriangle* tMin = *std::min_element(triangles.begin() + start, triangles.begin() + end, TrianglePointerComparator(i));
		SimpleTriangle* tMax = *std::max_element(triangles.begin() + start, triangles.begin() + end, TrianglePointerComparator(i));

		currentMin[i] = glm::min(tMin->a[i],glm::min(tMin->b[i], tMin->c[i]));
		currentMax[i] = glm::max(tMin->a[i], glm::max(tMin->b[i], tMin->c[i]));
	}

	//to calculate the span of the geometry
	glm::vec3 span = currentMax - currentMin;

	//to find the relevant dimension
	int j = span.x >= span.y ? (span.x >= span.z ? 0 : 2) : (span.y >= span.z ? 1 : 2);


	//find the median
	int range = end - start;
	int i = start + ((range) * 0.5f);
	std::nth_element(triangles.begin() + start, triangles.begin() + i, triangles.begin() + end, TrianglePointerComparator(j));
	SimpleTriangle* medianTriangle = triangles.at(i);
	glm::vec3 median = medianTriangle->a + medianTriangle->b + medianTriangle->c;
	median /= 3.0f;
	
	//if median is not clear use arithmetic mean
	if (range % 2 == 0) {
		std::nth_element(triangles.begin() + start, triangles.begin() + i + 1, triangles.begin() + end, TrianglePointerComparator(j));
		SimpleTriangle* medianTriangleB = triangles.at(i + 1);
		glm::vec3 medianB = medianTriangleB->a + medianTriangleB->b + medianTriangleB->c;
		medianB /= 3.0f;
		median += medianB;
		median /= 2.0f;
	}
	//insert a node using the median
	node = new Node(median[j], j);
	_nodes.push_back(node);
	//recursively insert more nodes
	insertTrianglesAsNode(node->Children[0], triangles, start, i, depth + 1, maxDepth);
	insertTrianglesAsNode(node->Children[1], triangles, i + 1, end, depth + 1, maxDepth);
	return;
}
//create nodes using a vector of vertices
void KDTree::insertVerticesAsNode(Node*& node, std::vector<glm::vec3> vertices, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth) {
	//break if maxdepth is reached (or there are no more vertices)
	if (start >= end || depth >= maxDepth) {
		return;
	}

	//calculate extreme values on all axis
	glm::vec3 currentMin;
	glm::vec3 currentMax;

	currentMin.x = std::min_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(0))->x;
	currentMin.y = std::min_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(1))->y;
	currentMin.z = std::min_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(2))->z;

	currentMax.x = std::max_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(0))->x;
	currentMax.y = std::max_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(1))->y;
	currentMax.z = std::max_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(2))->z;

	//to calculate the span of the geometry
	glm::vec3 span = currentMax - currentMin;
	//to find the relevant dimension
	int j = span.x >= span.y ? (span.x >= span.z ? 0 : 2) : (span.y >= span.z ? 1 : 2);

	//find the median
	int range = end - start;
	int i = start + ((range) * 0.5f);
	std::nth_element(vertices.begin() + start, vertices.begin() + i, vertices.begin() + end, Vec3Comparator(j));
	float median = glm::vec3(vertices.at(i))[j];

	//if median is not clear use arithmetic mean
	if (range % 2 == 0) {
		std::nth_element(vertices.begin() + start, vertices.begin() + i + 1, vertices.begin() + end, Vec3Comparator(j));
		float medianB = glm::vec3(vertices.at(i + 1))[j];
		median += medianB;
		median /= 2.0f;
	}
	//insert a node using the median
	node = new Node(median, j);
	_nodes.push_back(node);
	//recursively insert more nodes
	insertVerticesAsNode(node->Children[0],vertices, start, i, depth + 1, maxDepth);
	insertVerticesAsNode(node->Children[1], vertices, i + 1, end, depth + 1, maxDepth);
	return;
}


//insert a triangle into a leaf node
void KDTree::insertTriangleAsLeaf(Node* node, SimpleTriangle*& triangle) {
	
	//if node is a leaf node store the triangle
	if (node->Children[0] == nullptr && node->Children[1] == nullptr) {
		if (node->ChildTriangles == nullptr) {
			node->ChildTriangles = new std::vector<SimpleTriangle*>();
		}
		node->ChildTriangles->push_back(triangle);
		return;
	}

	//else we need to pass it down the tree
	bool a = node->Value > triangle->a[node->J];
	bool b = node->Value > triangle->b[node->J];
	bool c = node->Value > triangle->c[node->J];
	bool completelySmaller = (a && b && c);
	bool completelyBigger = (!a && !b && !c);
	//if it is completely smaller than the current node traverse left or if no left exist store it here
	if (completelySmaller) {
		insertTriangleAsLeaf(node->Children[0], triangle);
	}
	//if its completely bigger traverse right, or if no right exists store it here
	else if (completelyBigger) {
		insertTriangleAsLeaf(node->Children[1], triangle);
	}
	//if it is not completely clear send it down both paths and also store it here if one or both of those paths dont exist
	else {
		insertTriangleAsLeaf(node->Children[0], triangle);
		insertTriangleAsLeaf(node->Children[1], triangle);
	}
	return;
}

//prints statistics about the created nodes
void KDTree::printNodeStatistic()
{
	float sum = 0;
	int leafNodeAmount = 0;
	int maxChildNodeAmount = 0;
	int minChildNodeAmount = _triangles.size();
	for (auto n : _nodes) {
		if (n->ChildTriangles != nullptr) {
			sum += n->ChildTriangles->size();
			if (minChildNodeAmount > n->ChildTriangles->size()) {
				minChildNodeAmount = n->ChildTriangles->size();
			}
			if (maxChildNodeAmount < n->ChildTriangles->size()) {
				maxChildNodeAmount = n->ChildTriangles->size();
			}
		}

		if (n->Children[0] == nullptr || n->Children[1] == nullptr) {
			leafNodeAmount++;
		}
	}
	std::cout << "Total Triangles: " << sum << std::endl;
	std::cout << "Actual Triangle Amount: " << _triangles.size() << std::endl;
	std::cout << "Total Nodes created: " << _nodes.size() << std::endl;
	std::cout << "Leaf Nodes created: " << leafNodeAmount << std::endl;
	std::cout << "Max Child Triangles: " << maxChildNodeAmount << std::endl;
	std::cout << "Min Child Triangles: " << minChildNodeAmount << std::endl;
	float mean = sum / (float)leafNodeAmount;
	std::cout << "Mean Child Triangles: " << mean << std::endl;

	float varianceSum = 0;
	for (auto n : _nodes) {
		if (n->ChildTriangles != nullptr) {
			varianceSum += glm::pow((n->ChildTriangles->size()-mean),2);
		}
	}
	float variance = varianceSum / (float)leafNodeAmount;
	float stdVariance = glm::sqrt(variance);

	std::cout << "Standard Variance Child Triangles: " << stdVariance << std::endl;

}
#pragma endregion 

#pragma region Ray Intersection

//intersecting the scene with a ray
Hit* KDTree::RayIntersect(Ray ray)
{
#ifdef DEBUG_VISUALS
	for (auto t : _debugTriangles) {
		t->GetComponent<Triangle>()->Active = false;
	}
#endif // DEBUG_VISUALS
#ifdef DEBUG_INFORMATION
	auto start = std::chrono::high_resolution_clock::now();
#endif // DEBUG_INFORMATION

	Hit* hit = nullptr;
	//start recursion at root
	visitNodes(root, ray, hit);
	//reset triangle cache
	for (auto& t : _triangles) {
		t->checked = false;
	}

#ifdef DEBUG_INFORMATION
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Raycast with ray: " << RayToString(ray) << " took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds";
	if (hit != nullptr) {
		std::cout << " and hit at " << Vec3ToString(hit->point) << "." << std::endl;
	}
	else {
		std::cout << " and hit nothing." << std::endl;
	}
	return hit;
#endif // DEBUG_INFORMATION
}

void  KDTree::visitNodes(Node* node, Ray ray, Hit*& hit) {
	//exit condition
	if (node == nullptr) {
		return;
	}

	//intersect current node
	intersectNode(node, ray, hit);

	//find near child
	int dim = node->J;
	int first = ray.A[dim] > node->Value;
	//if the ray is parallel only visit near child
	if (ray.D[dim] == 0.0f) {
		visitNodes(node->Children[first], ray, hit);
	}
	//else find intersection
	else {
		//if intersection is inside range of ray visit both children
		float t = (node->Value - ray.A[dim]) / ray.D[dim];
		if (0.0f <= t && t < ray.TMax) {
			visitNodes(node->Children[first], ray, hit);
			visitNodes(node->Children[first ^ 1], ray, hit);
		}
		//else only visit near child
		else {
			visitNodes(node->Children[first], ray, hit);
		}
	}
}
//intersect a node with a ray
void KDTree::intersectNode(Node* node, Ray ray, Hit*& hit)
{
	//if node or childtriangles ar empty return
	if (node == nullptr || node->ChildTriangles == nullptr) {
		return;
	}
	//set current mininum found t value
	float curT = hit == nullptr ? ray.TMax : glm::distance(hit->point, ray.A);
	//iterate over all triangles
	for (auto& triangle : *(node->ChildTriangles)) {
		//dont check triangles twice
		if (!triangle->checked) {
#ifdef DEBUG_VISUALS
			triangle->debugTriangle->GetComponent<Triangle>()->Active = true;
#endif // DEBUG_VISUALS
			//intersect current triangle
			float t = intersectTriangle(triangle, ray);
			//if t is valid and smaller than current minium
			if (t >= 0 && curT >= t) {
				//change hit to represent new intersection
				curT = t;
				if (hit == nullptr) {
					hit = new Hit();
				}
				hit->Triangle = triangle;
				hit->point = ray.A + ray.D * t;
			}
			//mark triangle as checked
			triangle->checked = true;
		}
	}
}

//intersecting a triangle with a ray
float KDTree::intersectTriangle(SimpleTriangle*& triangle, Ray ray)
{
	//calculating normal of plane that includes the triangle
	glm::vec3 AB = triangle->b - triangle->a;
	glm::vec3 AC = triangle->c - triangle->a;
	glm::vec3 n = glm::cross(AB, AC);
	n = glm::normalize(n);

	//intersect ray with that plane
	float d = glm::dot(n, triangle->a);
	float denom = glm::dot(n, ray.D);
	if (denom == 0) {
		return -1;
	}
	float t = (d - glm::dot(n, ray.A)) / denom;

	//find intersection point with plane
	glm::vec3 Q = ray.A + ray.D * t;

	//check if that point is inside the triangle
	glm::vec3 BC = triangle->c - triangle->b;
	glm::vec3 CA = triangle->a - triangle->c;
	glm::vec3 AQ = Q - triangle->a;
	glm::vec3 BQ = Q - triangle->b;
	glm::vec3 CQ = Q - triangle->c;

	if (glm::dot(glm::cross(AB, AQ), n) < 0) {
		return -1;
	}
	if (glm::dot(glm::cross(BC, BQ), n) < 0) {
		return -1;
	}
	if (glm::dot(glm::cross(CA, CQ), n) < 0) {
		return -1;
	}

	return t;
}

#pragma endregion

#pragma region Visualisation

//generate lines visualising the KDTree
void KDTree::generateLines(VisualMode visualMode)
{
	switch (visualMode) {
	case VisualMode::BoundingBox:
		drawRecursiveBoundingBoxes(root);
		break;
	case VisualMode::GridPlane:
		glm::vec3 min = glm::vec3
		(
			findMinValueUnderNode(root, 0).x,
			findMinValueUnderNode(root, 1).y,
			findMinValueUnderNode(root, 2).z
		);
		glm::vec3 max = glm::vec3
		(
			findMaxPointFromNode(root, 0).x,
			findMaxPointFromNode(root, 1).y,
			findMaxPointFromNode(root, 2).z
		);
		drawRecursiveGridPlanes(root, min, max);
		break;
	}
}

//drawing a AABB
void KDTree::drawAABB(glm::vec3 min, glm::vec3 max, glm::vec4 color)
{
	//brute force finding of corners and then connecting them
	glm::vec3 a = min;
	glm::vec3 b = glm::vec3(max.x, min.y, min.z);
	glm::vec3 c = glm::vec3(max.x, min.y, max.z);
	glm::vec3 d = glm::vec3(min.x, min.y, max.z);
	glm::vec3 e = glm::vec3(min.x, max.y, min.z);
	glm::vec3 f = glm::vec3(max.x, max.y, min.z);
	glm::vec3 g = glm::vec3(max.x, max.y, max.z);
	glm::vec3 h = glm::vec3(min.x, max.y, max.z);

	Entity* ab = new Entity();
	ab->AddComponent<Line>();
	ab->GetComponent<Line>()->SetPoints(a, b);
	ab->GetComponent<Line>()->Color = color;
	_lines.push_back(ab);

	Entity* bc = new Entity();
	bc->AddComponent<Line>();
	bc->GetComponent<Line>()->SetPoints(b, c);
	bc->GetComponent<Line>()->Color = color;
	_lines.push_back(bc);

	Entity* cd = new Entity();
	cd->AddComponent<Line>();
	cd->GetComponent<Line>()->SetPoints(c, d);
	cd->GetComponent<Line>()->Color = color;
	_lines.push_back(cd);

	Entity* da = new Entity();
	da->AddComponent<Line>();
	da->GetComponent<Line>()->SetPoints(d, a);
	da->GetComponent<Line>()->Color = color;
	_lines.push_back(da);


	Entity* ef = new Entity();
	ef->AddComponent<Line>();
	ef->GetComponent<Line>()->SetPoints(e, f);
	ef->GetComponent<Line>()->Color = color;
	_lines.push_back(ef);


	Entity* fg = new Entity();
	fg->AddComponent<Line>();
	fg->GetComponent<Line>()->SetPoints(f, g);
	fg->GetComponent<Line>()->Color = color;
	_lines.push_back(fg);


	Entity* gh = new Entity();
	gh->AddComponent<Line>();
	gh->GetComponent<Line>()->SetPoints(g, h);
	gh->GetComponent<Line>()->Color = color;
	_lines.push_back(gh);


	Entity* he = new Entity();
	he->AddComponent<Line>();
	he->GetComponent<Line>()->SetPoints(h, e);
	he->GetComponent<Line>()->Color = color;
	_lines.push_back(he);

	Entity* ae = new Entity();
	ae->AddComponent<Line>();
	ae->GetComponent<Line>()->SetPoints(a, e);
	ae->GetComponent<Line>()->Color = color;
	_lines.push_back(ae);

	Entity* bf = new Entity();
	bf->AddComponent<Line>();
	bf->GetComponent<Line>()->SetPoints(b, f);
	bf->GetComponent<Line>()->Color = color;
	_lines.push_back(bf);

	Entity* cg = new Entity();
	cg->AddComponent<Line>();
	cg->GetComponent<Line>()->SetPoints(c, g);
	cg->GetComponent<Line>()->Color = color;
	_lines.push_back(cg);

	Entity* dh = new Entity();
	dh->AddComponent<Line>();
	dh->GetComponent<Line>()->SetPoints(d, h);
	dh->GetComponent<Line>()->Color = color;
	_lines.push_back(dh);
}

//drawing planes exactly at the splitting planes
void KDTree::drawRecursiveGridPlanes(Node* node, glm::vec3 min, glm::vec3 max)
{
	if (node == nullptr) {
		return;
	}
	//using previous maxima as cut off points
	glm::vec3 leftMin = min;
	glm::vec3 leftMax = max;
	glm::vec3 rightMin = min;
	glm::vec3 rightMax = max;

	//and changing relevant axis value to current node
	leftMax[node->J] = node->Value;
	rightMin[node->J] = node->Value;

	//Drawing and recursively going down the tree
	drawAABB(leftMin, leftMax, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	drawRecursiveGridPlanes(node->Children[0], leftMin, leftMax);

	drawAABB(rightMin, rightMax, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	drawRecursiveGridPlanes(node->Children[1], rightMin, rightMax);

	return;
}

//drawing bounding boxes for geometry under the node
void KDTree::drawRecursiveBoundingBoxes(Node* node)
{
	if (node == nullptr) {
		return;
	}


	if (node->ChildTriangles != nullptr) {
		//finding extrem values of child geometry
		glm::vec3 curMin = glm::vec3(findMinValueUnderNode(node, 0).x, findMinValueUnderNode(node, 1).y, findMinValueUnderNode(node, 2).z);
		glm::vec3 curMax = glm::vec3(findMaxPointFromNode(node, 0).x, findMaxPointFromNode(node, 1).y, findMaxPointFromNode(node, 2).z);;
		//drawing bounding box
		drawAABB(curMin, curMax, glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
	}
	//recursively going down the tree
	drawRecursiveBoundingBoxes(node->Children[0]);
	drawRecursiveBoundingBoxes(node->Children[1]);

	return;
}

//finding the lowest j value of vertices under node
glm::vec3 KDTree::findMinValueUnderNode(Node* node, unsigned int j)
{
	if (node == NULL) {
		return glm::vec3(INT_MAX);
	}

	//if node is using relevant j
	if (node->J == j) {
		//and we are already at a leaf node
		if (node->Children[0] == nullptr) {
			//check the geometry under it
			return findMinChildPoint(node, j);
		}
		//else recursively go down the left tree and find minima between left tree and this node
		else {
			glm::vec3 a = findMinChildPoint(node, j);
			glm::vec3 b = findMinValueUnderNode(node->Children[0], j);
			return a[j]<b[j] ? a : b;
		}
	}
	//if the node is not using a relevant j we have to check both children and find the minimas
	glm::vec3 a = findMinChildPoint(node, j);
	glm::vec3 b = findMinValueUnderNode(node->Children[0], j);
	glm::vec3 c = findMinValueUnderNode(node->Children[1], j);
	return a[j]<b[j]? (a[j]<c[j] ? a : c) : (b[j]<c[j] ? b : c);
}
//finds the lowest j value of geometry under node
glm::vec3 KDTree::findMinChildPoint(Node* node, unsigned int j) {
	//find the triangle with the lowest j component
	auto tri = findMinChildTriangle(node, j);
	if (tri == nullptr) {
		return glm::vec3(INT_MAX);
	}
	//find lowest value in the triangle
	glm::vec3 min = tri->a[j]<tri->b[j] ? (tri->a[j] < tri->c[j] ? tri->a : tri->c) : (tri->b[j] < tri->c[j] ? tri->b : tri->c);

	return min;
}
//finds the minimum triangle under node (using a custom comparator)
SimpleTriangle* KDTree::findMinChildTriangle(Node* node, unsigned int j) {
	if (node->ChildTriangles == nullptr) {
		return nullptr;
	}
	//find lowest triangle using std::min_element function and custom comparator
	auto index = std::min_element(node->ChildTriangles->begin(), node->ChildTriangles->end(), TrianglePointerComparator(j));
	return node->ChildTriangles->at(std::distance(node->ChildTriangles->begin(), index));
}

//finding the highest j value of vertices under node
glm::vec3 KDTree::findMaxPointFromNode(Node* node, unsigned int j)
{
	if (node == NULL) {
		return glm::vec3(INT_MIN);
	}

	//if node is using relevant j
	if (node->J == j) {
		//and we are already at a leaf node
		if (node->Children[1] == nullptr) {
			//check the geometry under it
			return findMaxChildPoint(node, j);
		}
		//else recursively go down the right tree and find maxima between right tree and this node
		else {
			glm::vec3 a = findMaxChildPoint(node, j);
			glm::vec3 b = findMaxPointFromNode(node->Children[1], j);
			return a[j]<b[j] ? b : a;
		}
	}

	//if the node is not using a relevant j we have to check both children and find the maximas
	glm::vec3 a = findMaxChildPoint(node, j);
	glm::vec3 b = findMaxPointFromNode(node->Children[0], j);
	glm::vec3 c = findMaxPointFromNode(node->Children[1], j);
	return a[j]<b[j] ? (b[j] < c[j] ? c : b) : (a[j] < c[j] ? c : a);
}
//finds the highest j value of geometry under node
glm::vec3 KDTree::findMaxChildPoint(Node* node, unsigned int j) {
	//find the triangle with the highest j component
	auto tri = findMaxChildTriangle(node, j);
	if (tri == nullptr) {
		return glm::vec3(INT_MIN);
	}
	//find highest value in the triangle
	glm::vec3 max = tri->a[j]<tri->b[j] ? (tri->b[j] < tri->c[j] ? tri->c : tri->b) : (tri->a[j] < tri->c[j] ? tri->c : tri->a);
	return max;
}
//finds the maximum triangle under node (using a custom comparator)
SimpleTriangle* KDTree::findMaxChildTriangle(Node* node, unsigned int j) {
	if (node->ChildTriangles == nullptr) {
		return nullptr;
	}
	//find highest triangle using std::max_element function and custom comparator
	auto index = std::max_element(node->ChildTriangles->begin(), node->ChildTriangles->end(), TrianglePointerComparator(j));
	return node->ChildTriangles->at(std::distance(node->ChildTriangles->begin(), index));
}

#pragma endregion