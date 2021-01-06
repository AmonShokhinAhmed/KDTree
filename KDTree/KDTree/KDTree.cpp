#include "KDTree.h"

KDTree::KDTree(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, unsigned int approxChildTris, bool visual)
{
	float depth = (float)indices.size() / 3.0f / (float)approxChildTris;
	depth = glm::log2(depth);
	depth+=1;
	depth = glm::ceil(depth);
	if (vertices.size() <= 0) {
		return;
	}
	insertVerticesAsNode(vertices, 0, vertices.size(), 0, depth);
	for (int i = 0; i < indices.size(); i += 3) {
		SimpleTriangle* triangle = new SimpleTriangle();
		triangle->a = vertices[indices[i]];
		triangle->b = vertices[indices[i + 1]];
		triangle->c = vertices[indices[i + 2]];
		triangle->checked = false;
		_triangles.push_back(triangle);
		insertTriangle(root, triangle);
	}
	if (visual) {
		generateLines();
	}
}

SimpleTriangle* KDTree::FindMinTriangle(unsigned int j)
{
	return findMinTriangleFromNode(root, j);
}
glm::vec3 KDTree::FindMinPoint(unsigned int j)
{
	return findMinPointFromNode(root, j);
}
glm::vec3 KDTree::FindMaxPoint(unsigned int j)
{
	return findMaxPointFromNode(root, j);
}

KDTree::~KDTree()
{
	for (auto e : _lines) {
		delete e;
	}
	_lines.clear();
}

SimpleTriangle* KDTree::findMinTriangleFromNode(Node* node, unsigned int j) {
	if (node == NULL) {
		SimpleTriangle* t = new SimpleTriangle();
		t->a = glm::vec3(INT_MAX);
		t->b = glm::vec3(INT_MAX);
		t->c = glm::vec3(INT_MAX);
		return t;
	}

	if (node->j == j) {
		if (node->left == nullptr) {
			return findMinChildTriangle(node, j);
		}
		else {
			SimpleTriangle* a = findMinChildTriangle(node, j);
			SimpleTriangle* b = findMinTriangleFromNode(node->left, j);
			return TriangleComparator(j)(a, b) ? a : b;
		}
	}

	SimpleTriangle* a = findMinChildTriangle(node, j);
	SimpleTriangle* b = findMinTriangleFromNode(node->left, j);
	SimpleTriangle* c = findMinTriangleFromNode(node->right, j);
	return TriangleComparator(j)(a, b) ? (TriangleComparator(j)(a, c) ? a : c) : (TriangleComparator(j)(b, c) ? b : c);
}
glm::vec3 KDTree::findMinPointFromNode(Node* node, unsigned int j)
{
	if (node == NULL) {
		return glm::vec3(INT_MAX);
	}

	if (node->j == j) {
		if (node->left == nullptr) {
			return findMinChildPoint(node, j);
		}
		else {
			glm::vec3 a = findMinChildPoint(node, j);
			glm::vec3 b = findMinPointFromNode(node->left, j);
			return Vec3Comparator(j)(a, b) ? a : b;
		}
	}

	glm::vec3 a = findMinChildPoint(node, j);
	glm::vec3 b = findMinPointFromNode(node->left, j);
	glm::vec3 c = findMinPointFromNode(node->right, j);
	return Vec3Comparator(j)(a, b) ? (Vec3Comparator(j)(a, c) ? a : c) : (Vec3Comparator(j)(b, c) ? b : c);
}
SimpleTriangle* KDTree::findMinChildTriangle(Node* node, unsigned int j) {
	if (node->childTriangles == nullptr) {
		SimpleTriangle* t = new SimpleTriangle();
		t->a = glm::vec3(INT_MAX);
		t->b = glm::vec3(INT_MAX);
		t->c = glm::vec3(INT_MAX);
		return t;
	}
	auto index = std::min_element(node->childTriangles->begin(), node->childTriangles->end(), TriangleComparator(j));
	return node->childTriangles->at(std::distance(node->childTriangles->begin(), index));
}
glm::vec3 KDTree::findMinChildPoint(Node* node, unsigned int j) {
	auto tri = findMinChildTriangle(node, j);
	glm::vec3 min = Vec3Comparator(j)(tri->a, tri->b) ? (Vec3Comparator(j)(tri->a, tri->c) ? tri->a : tri->c) : (Vec3Comparator(j)(tri->b, tri->c) ? tri->b : tri->c);
	return min;
}

glm::vec3 KDTree::findMaxPointFromNode(Node* node, unsigned int j)
{
	if (node == NULL) {
		return glm::vec3(INT_MIN);
	}

	if (node->j == j) {
		if (node->right == nullptr) {
			return findMaxChildPoint(node, j);
		}
		else {
			glm::vec3 a = findMaxChildPoint(node, j);
			glm::vec3 b = findMaxPointFromNode(node->right, j);
			return Vec3Comparator(j)(a, b) ? b : a;
		}
	}

	glm::vec3 a = findMaxChildPoint(node, j);
	glm::vec3 b = findMaxPointFromNode(node->left, j);
	glm::vec3 c = findMaxPointFromNode(node->right, j);
	return  Vec3Comparator(j)(a, b) ? (Vec3Comparator(j)(b, c) ? c : b) : (Vec3Comparator(j)(a, c) ? c : a);
}
glm::vec3 KDTree::findMaxChildPoint(Node* node, unsigned int j) {
	if (node->childTriangles == nullptr) {
		return glm::vec3(INT_MIN);
	}
	auto index = std::max_element(node->childTriangles->begin(), node->childTriangles->end(), TriangleComparator(j));
	auto tri = node->childTriangles->at(std::distance(node->childTriangles->begin(), index));
	glm::vec3 max = Vec3Comparator(j)(tri->a, tri->b) ? (Vec3Comparator(j)(tri->b, tri->c) ? tri->c : tri->b) : (Vec3Comparator(j)(tri->a, tri->c) ? tri->c : tri->a);
	return max;
}

static int intersectedTris = 0;
#pragma region Ray Intersection
Hit* KDTree::RayIntersect(Ray ray)
{
	intersectedTris = 0;
	Hit* hit = nullptr;
	visitNodes(root, ray, hit);
	for (auto& t : _triangles) {
		t->checked = false;
	}
	return hit;
}
void  KDTree::visitNodes(Node* node, Ray ray, Hit*& hit) {
	if (node == nullptr) {
		return;
	}
	if (hit != nullptr) {
		return;
	}
	intersectNode(node, ray, hit);

	int dim = node->j;
	int first = ray.A[dim] > node->vertex[dim];
	Node* children[] = { node->left, node->right };
	if (ray.D[dim] == 0.0f) {
		visitNodes(children[first], ray, hit);
	}
	else {
		float t = (node->vertex[dim] - ray.A[dim]) / ray.D[dim];
		if (0.0f <= t && t < ray.TMax) {

			Ray newRay;
			newRay.A = ray.A + t * ray.D;
			newRay.D = ray.D;
			newRay.TMax = ray.TMax - t;

			ray.TMax = t;

			visitNodes(children[first], ray, hit);
			visitNodes(children[first ^ 1], newRay, hit);
		}
		else {
			visitNodes(children[first], ray, hit);
		}
	}
}
void KDTree::intersectNode(Node* node, Ray ray, Hit*& hit)
{
	if (node == nullptr || hit != nullptr || node->childTriangles == nullptr) {
		return;
	}
	std::vector<Hit> hits;
	float curT = ray.TMax;
	for (auto& triangle : *(node->childTriangles)) {
		if (!triangle->checked) {
			float t = intersectTriangle(triangle, ray);
			if (t >= 0 && curT > t) {
				curT = t;
				if (hit == nullptr) {
					hit = new Hit();
				}
				hit->Triangle = triangle;
				hit->point = ray.A + ray.D * t;
			}
			triangle->checked = true;
		}
	}
}
float KDTree::intersectTriangle(SimpleTriangle*& triangle, Ray ray)
{
	intersectedTris++;
	glm::vec3 BA = triangle->a - triangle->b;
	glm::vec3 CA = triangle->a - triangle->c;
	glm::vec3 n = glm::cross(BA, CA);
	n = glm::normalize(n);
	float d = glm::dot(n, triangle->a);
	float denom = glm::dot(n, ray.D);
	if (denom == 0) {
		return -1;
	}
	float t = (d - glm::dot(n , ray.A)) / denom;
	glm::vec3 Q = ray.A + ray.D * t;

	glm::vec3 CB = triangle->b - triangle->c;
	glm::vec3 AC = triangle->c - triangle->a;
	glm::vec3 QA = triangle->a - Q;
	glm::vec3 QB = triangle->b - Q;
	glm::vec3 QC = triangle->c - Q;

	if (glm::dot(glm::cross(BA, QA), n) < 0) {
		return -1;
	}
	if (glm::dot(glm::cross(CB, QB), n) < 0) {
		return -1;
	}
	if (glm::dot(glm::cross(AC, QC), n) < 0) {
		return -1;
	}

	return t;
}
#pragma endregion

#pragma region Tree Building
void KDTree::insertVerticesAsNode(std::vector<glm::vec3> vertices, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth) {
	if (start >= end || depth >= maxDepth) {
		return;
	}

	glm::vec3 currentMin;
	glm::vec3 currentMax;

	currentMin.x = std::min_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(0))->x;
	currentMin.y = std::min_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(1))->y;
	currentMin.z = std::min_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(2))->z;

	currentMax.x = std::max_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(0))->x;
	currentMax.y = std::max_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(1))->y;
	currentMax.z = std::max_element(vertices.begin() + start, vertices.begin() + end, Vec3Comparator(2))->z;

	glm::vec3 span = currentMax - currentMin;

	int j = span.x >= span.y ? (span.x >= span.z ? 0 : 2) : (span.y >= span.z ? 1 : 2);


	glm::vec3 point;
	int range = end - start;
	int i = start + ((range) * 0.5f);
	std::nth_element(vertices.begin() + start, vertices.begin() + i, vertices.begin() + end, Vec3Comparator(j));
	point = glm::vec3(vertices.at(i));

	if (range % 2 == 0) {
		std::nth_element(vertices.begin() + start, vertices.begin() + i + 1, vertices.begin() + end, Vec3Comparator(j));
		glm::vec3 pointB = glm::vec3(vertices.at(i + 1));

		point[j] = (point[j] + pointB[j]) * 0.5f;
	}
	insertNode(root, point, j);
	insertVerticesAsNode(vertices, start, i, depth + 1, maxDepth);
	insertVerticesAsNode(vertices, i + 1, end, depth + 1, maxDepth);
	return;
}
void KDTree::insertNode(Node*& node, glm::vec3 vertex, unsigned int j)
{
	if (node == nullptr) {
		node = new Node();
		node->vertex = vertex;
		node->j = j;
		node->left = nullptr;
		node->right = nullptr;
		node->childTriangles = nullptr;
		//std::cout << " -> " << node << "." << std::endl;
		return;
	}

	//left is smaller
	if (Vec3Comparator(node->j)(vertex, node->vertex)) {
		//std::cout << " -> left ";
		insertNode(node->left, vertex, j);
	}
	else {
		//std::cout << " -> right ";
		insertNode(node->right, vertex, j);
	}
	return;
}
/*
void KDTree::insertPoint(Node* node, glm::vec3& point)
{
	if (node->left == nullptr && node->right == nullptr) {
		if (node->childPoints == nullptr) {
			node->childPoints = new std::vector<glm::vec3>();
		}
		node->childPoints->push_back(point);
		return;
	}

	//left is smaller
	if (Vec3Comparator(node->j)(point, node->vertex)) {
		//std::cout << " -> left ";
		if (node->left != nullptr) {
			insertPoint(node->left, point);
		}
		else {
			if (node->childPoints == nullptr) {
				node->childPoints = new std::vector<glm::vec3>();
			}
			node->childPoints->push_back(point);
			return;
		}
	}
	else {
		if (node->right != nullptr) {
			insertPoint(node->right, point);
		}
		else {
			if (node->childPoints == nullptr) {
				node->childPoints = new std::vector<glm::vec3>();
			}
			node->childPoints->push_back(point);
			return;
		}
	}
	return;
}*/
void KDTree::insertTriangle(Node* node, SimpleTriangle*& triangle) {
	if (node->left == nullptr && node->right == nullptr) {
		if (node->childTriangles == nullptr) {
			node->childTriangles = new std::vector<SimpleTriangle*>();
		}
		node->childTriangles->push_back(triangle);
		return;
	}


	bool a = node->vertex[node->j] > triangle->a[node->j];
	bool b = node->vertex[node->j] > triangle->b[node->j];
	bool c = node->vertex[node->j] > triangle->c[node->j];
	bool completelySmaller = (a && b && c);
	bool completelyBigger = (!a &&! b && !c);
	//left is smaller
	if (completelySmaller) {
		if (node->left != nullptr) {
			insertTriangle(node->left, triangle);
		}
		else {
			if (node->childTriangles == nullptr) {
				node->childTriangles = new std::vector<SimpleTriangle*>();
			}
			node->childTriangles->push_back(triangle);
			return;
		}
	}
	else if (completelyBigger) {
		if (node->right != nullptr) {
			insertTriangle(node->right, triangle);
		}
		else {
			if (node->childTriangles == nullptr) {
				node->childTriangles = new std::vector<SimpleTriangle*>();
			}
			node->childTriangles->push_back(triangle);
			return;
		}
	}
	else {
		if (node->left != nullptr) {
			insertTriangle(node->left, triangle);
		}
		if (node->right != nullptr) {
			insertTriangle(node->right, triangle);
		}
		if (node->right == nullptr || node->left == nullptr) {
			if (node->childTriangles == nullptr) {
				node->childTriangles = new std::vector<SimpleTriangle*>();
			}
			node->childTriangles->push_back(triangle);
			return;
		}
	}
	return;
}
#pragma endregion 

#pragma region Visualisation
void KDTree::generateLines()
{
	glm::vec3 min = glm::vec3
	(
		FindMinPoint(0).x,
		FindMinPoint(1).y,
		FindMinPoint(2).z
	);
	glm::vec3 max = glm::vec3
	(
		FindMaxPoint(0).x,
		FindMaxPoint(1).y,
		FindMaxPoint(2).z
	);
	drawGridPlanesForNode(root, min, max);
	//drawBoundingBoxForNode(root);
}
void KDTree::drawLinesForAABB(glm::vec3 min, glm::vec3 max, glm::vec4 color)
{
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
void KDTree::drawGridPlanesForNode(Node* node, glm::vec3 min, glm::vec3 max)
{
	//std::cout << "Depth " << depth<<std::endl;
	if (node == nullptr) {
		return;
	}
	//std::cout << "Drawing Splitting Lines for Vertex " << vec3ToString(node->vertex) << " on "<< (node->j == 0 ? "X" : node->j == 1 ? "Y" : "Z")<<"-Axis"<<" at Depth "<<depth<<std::endl;
	glm::vec3 leftMin = min;
	glm::vec3 leftMax;
	glm::vec3 rightMin;
	glm::vec3 rightMax = max;

	switch (node->j) {
	case 0:
		leftMax = glm::vec3(node->vertex.x, max.y, max.z);
		rightMin = glm::vec3(node->vertex.x, min.y, min.z);
		break;
	case 1:
		leftMax = glm::vec3(max.x, node->vertex.y, max.z);
		rightMin = glm::vec3(min.x, node->vertex.y, min.z);
		break;
	default:
		leftMax = glm::vec3(max.x, max.y, node->vertex.z);
		rightMin = glm::vec3(min.x, min.y, node->vertex.z);
		break;

	}

	drawLinesForAABB(leftMin, leftMax, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	drawGridPlanesForNode(node->left, leftMin, leftMax);

	drawLinesForAABB(rightMin, rightMax, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	drawGridPlanesForNode(node->right, rightMin, rightMax);

	return;
}
void KDTree::drawBoundingBoxForNode(Node* node)
{
	//std::cout << "Depth " << depth<<std::endl;
	if (node == nullptr) {
		return;
	}

	if (node->childTriangles != nullptr) {
		//std::cout << "Drawing Splitting Lines for Vertex " << vec3ToString(node->vertex) << " on "<< (node->j == 0 ? "X" : node->j == 1 ? "Y" : "Z")<<"-Axis"<<" at Depth "<<depth<<std::endl;
		glm::vec3 curMin = glm::vec3(findMinPointFromNode(node, 0).x, findMinPointFromNode(node, 1).y, findMinPointFromNode(node, 2).z);
		glm::vec3 curMax = glm::vec3(findMaxPointFromNode(node, 0).x, findMaxPointFromNode(node, 1).y, findMaxPointFromNode(node, 2).z);;

		drawLinesForAABB(curMin, curMax, glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
	}
	drawBoundingBoxForNode(node->left);
	drawBoundingBoxForNode(node->right);

	return;
}
#pragma endregion