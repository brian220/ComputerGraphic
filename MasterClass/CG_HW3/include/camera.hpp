#ifndef CAMERAH
#define CAMERAH

#include "ray.hpp"
#include "util.hpp"

#define PI 3.14

class camera {
public:

	camera() {}

	// vfov: vertical field of view in degree
	camera(vec3 lookfrom, vec3 lookat, vec3 up, float vfov, float aspect) {
		vec3 u, v, w;
		float theta = vfov * PI / 180;
		float half_height = tan(theta/2);
		float half_width = aspect * half_height;
		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(up, w));
		v = cross(w, u);
		lower_left_corner = vec3(-half_width, -half_height, -1.0);
		lower_left_corner = origin - half_width * u - half_height * v - w;
		horizontal = 2 * half_width * u;
		vertical = 2 * half_height * v;
		
		//Use a simple camera first
		/*
		lower_left_corner = vec3(-1.0, -1.0, -1.0);
		horizontal = vec3(2.0, 0.0, 0.0);
		vertical = vec3(0.0, 2.0, 0.0);
		origin = vec3(0.0, 0.0, 0.0);*/
	}

	inline ray get_ray(float s, float t) const {

		// write something here ...

		// return ray point to (s ,t, 1) for now
		// you may need to remove this line
		// return ray(origin, vec3(s, t, 1));
		return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
	}

	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 origin;
};

#endif
