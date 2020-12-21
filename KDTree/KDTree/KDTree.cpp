#include "KDTree.h"

KDTree::KDTree(std::vector<glm::vec3> vertices, bool visual)
{
	buildTree(vertices);
	if (visual) {
		generateLines();
	}
}

KDTree::~KDTree()
{
	for (auto e : _lines) {
			delete e;
	}
	_lines.clear();
}

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

void KDTree::buildTree(std::vector<glm::vec3>vertices)
{
    if (vertices.size() <= 0) {
        return;
    }
    min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    root = new Node();
    root->j = 0;
    root->vertex = vertices[0];
    root->left = nullptr;
    root->right = nullptr;
    for (auto v : vertices) {
        insertNode(root, v, 0);
        for (auto v : vertices) {
            min.x = min.x > v.x ? v.x : min.x;
            min.y = min.y > v.y ? v.y : min.y;
            min.z = min.z > v.z ? v.z : min.z;

            max.x = max.x < v.x ? v.x : max.x;
            max.y = max.y < v.y ? v.y : max.y;
            max.z = max.z < v.z ? v.z : max.z;
        }
    }
}

void KDTree::generateLines()
{
    std::cout << "Generating Lines..." << std::endl;
    drawLinesForAABB(min, max);
    std::cout << "Generated Boundaries" << std::endl;
    drawLinesForNode(root, min, max, 0, 5);
}

void KDTree::drawLinesForAABB(glm::vec3 min, glm::vec3 max)
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
    _lines.push_back(ab);

    Entity* bc = new Entity();
    bc->AddComponent<Line>();
    bc->GetComponent<Line>()->SetPoints(b, c);
    _lines.push_back(bc);

    Entity* cd = new Entity();
    cd->AddComponent<Line>();
    cd->GetComponent<Line>()->SetPoints(c, d);
    _lines.push_back(cd);

    Entity* da = new Entity();
    da->AddComponent<Line>();
    da->GetComponent<Line>()->SetPoints(d, a);
    _lines.push_back(da);


    Entity* ef = new Entity();
    ef->AddComponent<Line>();
    ef->GetComponent<Line>()->SetPoints(e, f);
    _lines.push_back(ef);


    Entity* fg = new Entity();
    fg->AddComponent<Line>();
    fg->GetComponent<Line>()->SetPoints(f, g);
    _lines.push_back(fg);


    Entity* gh = new Entity();
    gh->AddComponent<Line>();
    gh->GetComponent<Line>()->SetPoints(g, h);
    _lines.push_back(gh);


    Entity* he = new Entity();
    he->AddComponent<Line>();
    he->GetComponent<Line>()->SetPoints(h, e);
    _lines.push_back(he);

    Entity* ae = new Entity();
    ae->AddComponent<Line>();
    ae->GetComponent<Line>()->SetPoints(a, e);
    _lines.push_back(ae);

    Entity* bf = new Entity();
    bf->AddComponent<Line>();
    bf->GetComponent<Line>()->SetPoints(b, f);
    _lines.push_back(bf);

    Entity* cg = new Entity();
    cg->AddComponent<Line>();
    cg->GetComponent<Line>()->SetPoints(c, g);
    _lines.push_back(cg);

    Entity* dh = new Entity();
    dh->AddComponent<Line>();
    dh->GetComponent<Line>()->SetPoints(d, h);
    _lines.push_back(dh);
}

void KDTree::drawLinesForNode(Node* node, glm::vec3 min, glm::vec3 max, unsigned int depth, unsigned int maxDepth)
{
    if (node == nullptr||depth>=maxDepth) {
        return;
    }
    //std::cout << "Drawing Splitting Line for Vertex " << node->vertex.x<<"|"<<node->vertex.y<<"|"<<node->vertex.z << " on "<< (node->j == 0 ? "X" : node->j == 1 ? "Y" : "Z")<<"-Axis"<<" at Depth "<<depth<<std::endl;
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
    //std::cout << "leftMax: " << leftMax.x << "|" << leftMax.y << "|" << leftMax.z << std::endl;
    //std::cout << "rightMin: " << rightMin.x << "|" << rightMin.y << "|" << rightMin.z << std::endl;
    //calculate
    drawLinesForAABB(leftMin, leftMax);
    drawLinesForNode(node->left, leftMin, leftMax, depth + 1, maxDepth);


    //calculate
    drawLinesForAABB(rightMin, rightMax);
    drawLinesForNode(node->right, rightMin, rightMax, depth + 1, maxDepth);
    
    return;
}

Node* KDTree::insertNode(Node* root, glm::vec3 vertex, unsigned int j)
{

    if (root == nullptr) {
        Node* node = new Node();
        node->vertex = vertex;
        node->j = j;
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }


    //left is smaller
    if (compareVertices(vertex, root->vertex, j)<0)
        root->left = insertNode(root->left, vertex, (j+1)%3);
    else
        root->right = insertNode(root->right, vertex, (j + 1) % 3);

    return root;
}

