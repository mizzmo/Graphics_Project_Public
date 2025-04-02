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

float* MakeFloatsFromVector(std::vector<point> curve, int &num_verts, int &num_floats, float r, float g, float b)
{
    num_verts = curve.size();
    if (num_verts == 0) {
        return NULL;
    }
    num_floats = num_verts * 6;

    float* attrib = (float*)malloc(num_floats*sizeof(float));
    int offset = 0;

    for (point p : curve) {
        attrib[offset + 0] = p.x;
        attrib[offset + 1] = p.y;
        attrib[offset + 2] = p.z;
        attrib[offset + 3] = r;
        attrib[offset + 4] = g;
        attrib[offset + 5] = b;
        offset += 6;
    }

	return attrib;
}

