#include "transform.hpp"

vec3 Translate(vec3 target, vec3 displacement) {
	vec3 moveTarget = target + displacement;
	return moveTarget;
}

vec3 Rotate(vec3 target, vec3 axis, float angle) {
	vec3 rotateTarget = target;

	float PI = 3.14;
	float radians = (PI / 180.) * angle;

	float x = target[0];
	float y = target[1];
	float z = target[2];

	float sin_theta = sin(radians);
	float cos_theta = cos(radians);

	// Rotate about y axis
	if (axis[1] == 1) {
		float newX = cos_theta * x + sin_theta * z;
		float newZ = -sin_theta * x + cos_theta * z;
		rotateTarget = vec3(newX, y, newZ);
	}

	return rotateTarget;
}

bool transform::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	// For translate
	if (rotateAngle == 0.0) {
		// Apply inverse tranlate on ray
		ray moved_r(r.origin() - displacement, r.direction());
		if (target->hit(moved_r, t_min, t_max, rec)) {
			// Re-apply translate on hitpoint and normal
			rec.p = Translate(rec.p, displacement);
			rec.normal = Translate(rec.normal, displacement);
			return true;
		}
		else
			return false;
	} // For rotate
	else {
		// Apply inverse rotate on ray
		vec3 origin = Rotate(r.origin(), rotateAxis, -1 * rotateAngle);
		vec3 direction = Rotate(r.direction(), rotateAxis, -1 * rotateAngle);
		ray rotated_r(origin, direction);
		if (target->hit(rotated_r, t_min, t_max, rec)) {
			rec.p = Rotate(rec.p, rotateAxis, rotateAngle);
			rec.normal = Rotate(rec.normal, rotateAxis, rotateAngle);
			return true;
		}
		else
			return false;
	}
	return false;
}