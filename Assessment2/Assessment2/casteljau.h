#include <list>
#include <vector>
#include <algorithm>

#include "point.h"

point evaluate(float t, std::list<point> P)
{
    std::list<point> Q = P;

    while (Q.size() > 1) {
        std::list<point> R;
        auto p1 = Q.begin();
        auto p2 = std::next(p1);

        while (p2 != Q.end()) {
            point p = ((1 - t) * (*p1)) + (t * (*p2));
            R.push_back(p);
            ++p1;
            ++p2;
        }
        Q.clear();
        Q = R;
    }
    return Q.front();
}


std::vector<point> EvaluateBezierCurve(std::vector<point>ctrl_points, int num_evaluations)
{
	// Copy control points argument to a list
	std::list<point> ps(ctrl_points.begin(), ctrl_points.end());
	std::vector<point> curve;

	float offset = 1.f / num_evaluations;

	curve.push_back(ps.front());

	for (int e = 0; e < num_evaluations; e++) {
		point p = evaluate(offset * (e + 1), ps);
		curve.push_back(p);
	}
	// The curve which is returned is a vector of points.
	return curve;
}

std::vector<GLfloat> MakeFloatsFromVector(const std::vector<point>& points, float r, float g, float b) 
{
    std::vector<GLfloat> vertices;
    // 6 floats per point
    vertices.reserve(points.size() * 6);

    for (const auto& p : points) {
        // Position 
        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(p.z);
        // Colour 
        vertices.push_back(r);
        vertices.push_back(g);
        vertices.push_back(b);
    }

    return vertices;
}

