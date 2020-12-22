#include "KDTree.h"
#include <sstream>
#include <iomanip>

int KDTree::compareVertices(glm::vec3 a, glm::vec3 b, unsigned int j)
{
    switch (j) {
    case 0:
        return a.x == b.x ? 0 : (a.x < b.x ? -1 : 1);
        break;
    case 1:
        return a.y == b.y ? 0 : (a.y < b.y ? -1 : 1);
        break;
    default:
        return a.z == b.z ? 0 : (a.z < b.z ? -1 : 1);
        break;
    }
}
static bool xCompare(glm::vec3 a, glm::vec3 b)
{
    return (a.x < b.x);
}
static bool yCompare(glm::vec3 a, glm::vec3 b)
{
    return (a.y < b.y);
}
static bool zCompare(glm::vec3 a, glm::vec3 b)
{
    return (a.z < b.z);
}
static string vec3ToString(glm::vec3 v)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << v.x << "|" << v.y << "|" << v.z;
    return stream.str();
}


KDTree::KDTree(std::vector<glm::vec3> vertices, bool visual)
{
	buildTree(vertices);
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


void KDTree::buildTree(std::vector<glm::vec3>vertices)
{
    if (vertices.size() <= 0) {
        return;
    }
    min = glm::vec3(0,0,0);
    max = glm::vec3(0,0,0);

    min.x = std::min_element(vertices.begin(), vertices.end(), xCompare)->x;
    min.y = std::min_element(vertices.begin(), vertices.end(), yCompare)->y;
    min.z = std::min_element(vertices.begin(), vertices.end(), zCompare)->z;

    max.x = std::max_element(vertices.begin(), vertices.end(), xCompare)->x;
    max.y = std::max_element(vertices.begin(), vertices.end(), yCompare)->y;
    max.z = std::max_element(vertices.begin(), vertices.end(), zCompare)->z;

    
    insertVertices(vertices, 0, vertices.size());
}


void KDTree::insertVertices(std::vector<glm::vec3> vertices, unsigned int start, unsigned int end) {
    if (start >= end) {
        return;
    }

    //std::cout << std::endl << "--------------------------------------------------------------------------------------" << std::endl << std::endl;
    glm::vec3 currentMin;
	glm::vec3 currentMax;

	currentMin.x = std::min_element(vertices.begin() + start, vertices.begin() + end, xCompare)->x;
	currentMin.y = std::min_element(vertices.begin() + start, vertices.begin() + end, yCompare)->y;
	currentMin.z = std::min_element(vertices.begin() + start, vertices.begin() + end, zCompare)->z;

	currentMax.x = std::max_element(vertices.begin() + start, vertices.begin() + end, xCompare)->x;
	currentMax.y = std::max_element(vertices.begin() + start, vertices.begin() + end, yCompare)->y;
	currentMax.z = std::max_element(vertices.begin() + start, vertices.begin() + end, zCompare)->z;

    //for (int i = start; i < end; i++) {
    //    std::cout << vec3ToString(vertices.at(i)) << std::endl;
    //}
    //std::cout << std::endl;
	glm::vec3 span = currentMax - currentMin;
	//std::cout << "Span: " << vec3ToString(span);
	int j = span.x >= span.y ? (span.x >= span.z ? 0 : 2) : (span.y >= span.z ? 1 : 2);
	//std::cout << " therefore j is " << j << std::endl;
	int i = start + ((end - start) / 2);
	std::nth_element(vertices.begin() + start, vertices.begin() + i, vertices.begin() + end, j == 0 ? xCompare : (j == 1 ? yCompare : zCompare));


	//std::cout << std::endl << "after nth element: " << std::endl;
	//for (int i = start; i < end; i++) {
	//	std::cout << vec3ToString(vertices.at(i)) << std::endl;
	//}
	//std::cout << "Median is " << vec3ToString(vertices.at(i)) << std::endl;

    //std:cout << "Inserting vertex: " << vec3ToString(vertices.at(i));
	insertNode(root, vertices.at(i), j);
	insertVertices(vertices, start, i);
	insertVertices(vertices, i + 1, end);
	return;
}

void KDTree::generateLines()
{
    //std::cout << "Generating Lines" << std::endl;
    //drawBoundingBoxForNode(root, 0, 3);
    drawGridPlanesForNode(root,min,max ,0, 3);
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

void KDTree::drawGridPlanesForNode(Node* node, glm::vec3 min, glm::vec3 max, unsigned int depth, unsigned int maxDepth)
{
    //std::cout << "Depth " << depth<<std::endl;
    if (node == nullptr || depth >= maxDepth) {
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
    drawGridPlanesForNode(node->left, leftMin, leftMax, depth + 1, maxDepth);

    drawLinesForAABB(rightMin, rightMax, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    drawGridPlanesForNode(node->right, rightMin, rightMax, depth + 1, maxDepth);

    return;
}


void KDTree::drawBoundingBoxForNode(Node* node, unsigned int depth, unsigned int maxDepth)
{
    //std::cout << "Depth " << depth<<std::endl;
    if (node == nullptr || depth >= maxDepth) {
        return;
    }
    //std::cout << "Drawing Splitting Lines for Vertex " << vec3ToString(node->vertex) << " on "<< (node->j == 0 ? "X" : node->j == 1 ? "Y" : "Z")<<"-Axis"<<" at Depth "<<depth<<std::endl;
    glm::vec3 curMin = glm::vec3(findMinFromNode(node, 0).x, findMinFromNode(node, 1).y, findMinFromNode(node, 2).z);
    glm::vec3 curMax = glm::vec3(findMaxFromNode(node, 0).x, findMaxFromNode(node, 1).y, findMaxFromNode(node, 2).z);;

    drawLinesForAABB(curMin, curMax, glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
    drawBoundingBoxForNode(node->left, depth + 1, maxDepth);

    drawBoundingBoxForNode(node->right, depth + 1, maxDepth);

    return;
}

glm::vec3 KDTree::findMinFromNode(Node* node, unsigned int j)
{
    if (node == NULL) {
        return glm::vec3(INT_MAX);
    }

    if (node->j == j) {
        if (node->left == nullptr){
            return node->vertex;
        }
        else {
            glm::vec3 a = node->vertex;
            glm::vec3 b = findMinFromNode(node->left, j);
            switch (j) {
            case 0:
                return xCompare(a, b) ? a : b;
                break;
            case 1:
                return yCompare(a, b) ? a : b;
                break;
            default:
                return zCompare(a, b) ? a : b;
                break;
            }
        }
    }

    glm::vec3 a = node->vertex;
    glm::vec3 b = findMinFromNode(node->left, j);
    glm::vec3 c = findMinFromNode(node->right, j);
    switch (j) {
    case 0:
        return xCompare(a, b) ? (xCompare(a,c)?a:c) : (xCompare(b,c)?b:c);
        break;
    case 1:
        return yCompare(a, b) ? (yCompare(a, c) ? a : c) : (yCompare(b, c) ? b : c);
        break;
    default:
        return zCompare(a, b) ? (zCompare(a, c) ? a : c) : (zCompare(b, c) ? b : c);
        break;
    }
}

glm::vec3 KDTree::findMaxFromNode(Node* node, unsigned int j)
{
    if (node == NULL) {
        return glm::vec3(INT_MIN);
    }

    if (node->j == j) {
        if (node->right == nullptr) {
            return node->vertex;
        }
        else {
            glm::vec3 a = node->vertex;
            glm::vec3 b = findMaxFromNode(node->right, j);
            switch (j) {
            case 0:
                return xCompare(a, b) ? b : a;
                break;
            case 1:
                return yCompare(a, b) ? b : a;
                break;
            default:
                return zCompare(a, b) ? b : a;
                break;
            }
        }
    }

    glm::vec3 a = node->vertex;
    glm::vec3 b = findMaxFromNode(node->left, j);
    glm::vec3 c = findMaxFromNode(node->right, j);
    switch (j) {
    case 0:
        return xCompare(a, b) ? (xCompare(b, c) ? c : b) : (xCompare(a, c) ? c : a);
        break;
    case 1:
        return yCompare(a, b) ? (yCompare(b, c) ? c : b) : (yCompare(a, c) ? c : a);
        break;
    default:
        return zCompare(a, b) ? (zCompare(b, c) ? c : b) : (zCompare(a, c) ? c : a);
        break;
    }
}

Node* KDTree::insertNode(Node*& node, glm::vec3 vertex, unsigned int j)
{
    if (node == nullptr) {
        node = new Node();
        node->vertex = vertex;
        node->j = j;
        node->left = nullptr;
        node->right = nullptr;
        //std::cout << " -> " << node << "." << std::endl;
        return node;
    }

    //left is smaller
    if (compareVertices(vertex, node->vertex, node->j) < 0) {
        //std::cout << " -> left ";
        insertNode(node->left, vertex, j);
	}
	else {
		//std::cout << " -> right ";
        insertNode(node->right, vertex, j);
	}

	return node;
}

