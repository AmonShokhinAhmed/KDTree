#include "KDTree.h"

KDTree::KDTree(std::vector<glm::vec3> vertices, unsigned int depth, bool visual)
{
    if (vertices.size() <= 0) {
        return;
    }
    insertVerticesAsNode(vertices, 0, vertices.size(),0,depth);
    for (auto v : vertices) {
        insertPoint(root, v);
    }
	if (visual) {
		generateLines();
	}
}

glm::vec3 KDTree::FindMin(unsigned int j)
{
    return findMinFromNode(root, j);
}
glm::vec3 KDTree::FindMax(unsigned int j)
{
    return findMaxFromNode(root,j);
}

KDTree::~KDTree()
{
	for (auto e : _lines) {
			delete e;
	}
	_lines.clear();
}

glm::vec3 KDTree::findMinFromNode(Node* node, unsigned int j)
{
    if (node == NULL) {
        return glm::vec3(INT_MAX);
    }

    if (node->j == j) {
        if (node->left == nullptr) {
            return findMinChildPoint(node,j);
        }
        else {
            glm::vec3 a = findMinChildPoint(node, j);
            glm::vec3 b = findMinFromNode(node->left, j);
            return Vec3Comparator(j)(a, b) ? a : b;
        }
    }

    glm::vec3 a = findMinChildPoint(node, j);
    glm::vec3 b = findMinFromNode(node->left, j);
    glm::vec3 c = findMinFromNode(node->right, j);
    return Vec3Comparator(j)(a, b) ? (Vec3Comparator(j)(a, c) ? a : c) : (Vec3Comparator(j)(b, c) ? b : c);
}
glm::vec3 KDTree::findMinChildPoint(Node* node, unsigned int j) {
    if (node->childPoints == nullptr) {
        return glm::vec3(INT_MAX);
    }
    auto index = std::min_element(node->childPoints->begin(), node->childPoints->end(), Vec3Comparator(j));
    return node->childPoints->at(std::distance(node->childPoints->begin(), index));
}
glm::vec3 KDTree::findMaxFromNode(Node* node, unsigned int j)
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
            glm::vec3 b = findMaxFromNode(node->right, j);
            return Vec3Comparator(j)(a, b) ? b : a;
        }
    }

    glm::vec3 a = findMaxChildPoint(node, j);
    glm::vec3 b = findMaxFromNode(node->left, j);
    glm::vec3 c = findMaxFromNode(node->right, j);
    return  Vec3Comparator(j)(a, b) ? (Vec3Comparator(j)(b, c) ? c : b) : (Vec3Comparator(j)(a, c) ? c : a);
}
glm::vec3 KDTree::findMaxChildPoint(Node* node, unsigned int j) {
    if (node->childPoints == nullptr) {
        return glm::vec3(INT_MIN);
    }
    auto index = std::max_element(node->childPoints->begin(), node->childPoints->end(), Vec3Comparator(j));
    return node->childPoints->at(std::distance(node->childPoints->begin(), index));
}

#pragma region Tree Building
void KDTree::insertVerticesAsNode(std::vector<glm::vec3> &vertices, unsigned int start, unsigned int end, unsigned depth, unsigned int maxDepth) {
    if (start >= end ||depth>=maxDepth) {
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

        point[j] = (point[j]+pointB[j])*0.5f;
    }
	insertNode(root, point, j);
	insertVerticesAsNode(vertices, start, i,depth+1,maxDepth);
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
        node->childPoints = nullptr;
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
                node->childPoints == new std::vector<glm::vec3>();
            }
            node->childPoints->push_back(point);
            return;
        }
    }
    return;
}
#pragma endregion 

#pragma region Visualisation
void KDTree::generateLines()
{
    //std::cout << "Generating Lines" << std::endl;
    //drawBoundingBoxForNode(root, 0, 3);

	glm::vec3 min = glm::vec3
    (
        FindMin(0).x,
        FindMin(1).y,
        FindMin(2).z
    );
	glm::vec3 max = glm::vec3
    (
        FindMax(0).x,
        FindMax(1).y,
        FindMax(2).z
    );
    //drawGridPlanesForNode(root,min,max);
    drawBoundingBoxForNode(root);
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
    drawGridPlanesForNode(node->left, leftMin, leftMax );

    drawLinesForAABB(rightMin, rightMax, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    drawGridPlanesForNode(node->right, rightMin, rightMax);

    return;
}
void KDTree::drawBoundingBoxForNode(Node* node)
{
    //std::cout << "Depth " << depth<<std::endl;
    if (node == nullptr ) {
		return;
	}

	if (node->childPoints != nullptr) {
		//std::cout << "Drawing Splitting Lines for Vertex " << vec3ToString(node->vertex) << " on "<< (node->j == 0 ? "X" : node->j == 1 ? "Y" : "Z")<<"-Axis"<<" at Depth "<<depth<<std::endl;
		glm::vec3 curMin = glm::vec3(findMinFromNode(node, 0).x, findMinFromNode(node, 1).y, findMinFromNode(node, 2).z);
		glm::vec3 curMax = glm::vec3(findMaxFromNode(node, 0).x, findMaxFromNode(node, 1).y, findMaxFromNode(node, 2).z);;

		drawLinesForAABB(curMin, curMax, glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
	}
	drawBoundingBoxForNode(node->left);

	drawBoundingBoxForNode(node->right);

    return;
}
#pragma endregion



