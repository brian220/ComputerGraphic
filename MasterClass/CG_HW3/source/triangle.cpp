#include "triangle.hpp"

bool triangle::hit(const ray& _ray, float t_min, float t_max, hit_record& rec) const
{   
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = cross(_ray.direction(), v0v2);
	float det = dot(v0v1, pvec);

	// Face Culling
	static const float kEpsilon = std::numeric_limits<float>::epsilon();
	if (det < kEpsilon) return false;

	float invDet = 1.0 / det;

	vec3 tvec = _ray.origin() - v0;
	float u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;

	vec3 qvec = cross(tvec, v0v1);
	float v = dot(_ray.direction(), qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	float temp = dot(v0v2, qvec) * invDet;

	if (temp < t_max && temp > t_min) {
		rec.t = temp;
		rec.p = _ray.point_at_parameter(rec.t);
		rec.normal = normal;
		rec.mat_ptr = triangle_mat;
		return true;
	}
	return false;
}