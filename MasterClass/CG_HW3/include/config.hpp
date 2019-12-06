#ifndef CONFIGH
#define CONFIGH

#include "util.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include "scene.hpp"

static unsigned long long seed = 1;

class config
{
public:
	config() {

		// Bunny Model
		std::vector<triangle> bunny_list = LoadModel("./Resources/bunnyLow.obj");
		material *bunny_material = new lambertian(vec3(1, 1, 1));

		vec3 displacement = vec3(0.05, 1, -1);
		vec3 rotateAxis(0, 1, 0);
		float rotateAngle = 180;

		for (int i = 0; i < bunny_list.size(); ++i) {
			list.push_back(
				new transform(
					new transform(
						new triangle(
							bunny_list[i].v0,
							bunny_list[i].v1,
							bunny_list[i].v2,
							bunny_material
						),
						vec3(0, 0, 0), rotateAxis, rotateAngle // rotate
					),
					displacement, vec3(0, 1, 0), 0 // translate
				)
			);
		}

		// Light Ball
		list.push_back(new sphere(vec3(0, 0, -1), 0.5, new light(vec3(1, 1, 1))));

		// Ground
		list.push_back(new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0))));

		// Metal Ball (Copper, fuzz = 0.3)
		list.push_back(new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(184, 115, 51) / 255.0, 0.3)));

		// Glass Ball (index = 1.4)
		list.push_back(new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.4)));

		// Blue Ball, equal to list.push_back(new sphere(vec3(1, 1, -1), 0.5, new lambertian(vec3(115, 190, 255) / 255.0)));
		list.push_back(
			new transform(
				new sphere(vec3(0, 0, 0), 0.5, new lambertian(vec3(115, 190, 255) / 255.0)),
				vec3(1, 1, -1), vec3(0, 1, 0), 0 // translate
			)
		);

		// textured Ball
		texture* _image_texture = new image_texture("./Resources/texture.jpg");
		list.push_back(new sphere(vec3(-1, 1, -1), 0.5, new lambertian(_image_texture)));

		// Create hitable_list
		world = new scene(list);

		// Setup Camera
		vec3 lookfrom(0, 0, 3.5);
		vec3 lookat(0, 0, 0);

		cam = camera(lookfrom, lookat, vec3(0, 1, 0), 45, float(width) / float(height));
	}

	~config()
	{
		delete world;
	}

	std::vector<hitable*> list;
	scene *world = nullptr;

	camera cam;

	int width = 512;
	int height = 512;
	int samples = 10000;
};

#endif