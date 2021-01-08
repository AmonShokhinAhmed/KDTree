#pragma once
#include <glm/glm.hpp>
#include "KDTree.h"
#include <string>
#include <sstream>


struct Vec3Comparator {
	Vec3Comparator(unsigned int j) :J(j) { }
	bool operator () (glm::vec3 a, glm::vec3 b) {
		return a[J] < b[J];
	}
	int J;
};
struct TrianglePointerAbsolutComparator {
	TrianglePointerAbsolutComparator(unsigned int j) :J(j) { }
	bool operator () (SimpleTriangle* triA, SimpleTriangle* triB) {
		float aMin = glm::min(glm::min(triA->a[J], triA->b[J]), triA->c[J]);
		float bMin = glm::min(glm::min(triB->a[J], triB->b[J]), triB->c[J]);
		return (aMin < bMin);
	}
	int J;
}; 
struct TrianglePointerCentroidComparator {
	TrianglePointerCentroidComparator(unsigned int j) :J(j) { }
	bool operator () (SimpleTriangle* triA, SimpleTriangle* triB) {
		float centroidA = triA->a[J] + triA -> b[J] + triA->c[J];
		centroidA /= 3.0f;
		float centroidB = triB->a[J] + triB->b[J] + triB->c[J];
		centroidB /= 3.0f;
		return (centroidA < centroidB);
	}
	int J;
};
struct TriangleComparator {
	TriangleComparator(unsigned int j) :J(j) { }
	bool operator () (SimpleTriangle triA, SimpleTriangle triB) {
		float aMin = glm::min(glm::min(triA.a[J], triA.b[J]), triA.c[J]);
		float bMin = glm::min(glm::min(triB.a[J], triB.b[J]), triB.c[J]);
		return (aMin < bMin);
	}
	int J;
};

static std::string Vec3ToString(glm::vec3 v)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << "(" << v.x << "|" << v.y << "|" << v.z << ")";
	return stream.str();
}

static std::string RayToString(Ray r)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << Vec3ToString(r.A) << " (" << r.TMax << ")-> " << Vec3ToString(r.D);
	return stream.str();
}

static std::string SimpleTriangleToString(SimpleTriangle triangle)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) <<"a="<< Vec3ToString(triangle.a) << ", b="<<Vec3ToString(triangle.b) << ", c="<<Vec3ToString(triangle.c);
	return stream.str();
}
