#include <iostream>
#include <string>
#include <limits> // for kEpsilon and kMaxFloat
#include <Windows.h> // for CreateDirectory

#include "config.hpp"
#include "random.hpp"

namespace {
	// do not modify this
	const size_t max_depth = 50;
}

vec3 color(const ray& r, hitable *world, size_t depth) {

	static const float kEpsilon = std::numeric_limits<float>::epsilon();
	static const float kMaxFloat = std::numeric_limits<float>::infinity();

	hit_record rec;

	if (world->hit(r, kEpsilon, kMaxFloat, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted();
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * color(scattered, world, depth + 1);
		}
		else {
			return emitted;
		}
	}
	else {
		// INFO : after you implement light material, uncomment the following
		// line for better visual comparison.
		// return vec3(0.0f, 0.0f, 0.0f);

		// hit nothing, return color of skybox
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main(int argc, char**argv)
{
	config sceneConfig = config();

	scene* world = sceneConfig.world;
	camera cam = sceneConfig.cam;

	int nx = sceneConfig.width;
	int ny = sceneConfig.height;
	int ns = sceneConfig.samples;

	// Do not touch this unless you are not using Windows
	// The below code simply creates a folder named "output" if it does not exists
	LPCWSTR OutputFolder = L"output";
	if (!(CreateDirectory(OutputFolder, NULL) || ERROR_ALREADY_EXISTS == GetLastError())) {
		std::cerr << "Failed to create directory" << std::endl;
		exit(1);
	}

	// initialize buffers to accumulate radiance
	float* image = new float[ny * nx * 3];	
	for (int j = 0; j < ny; ++j) {
		for (int i = 0; i < nx; ++i) {
			for (int n = 0; n < 3; ++n) {
				image[(j * nx + i) * 3 + n] = 0.0f;
			}
		}
	}

	// start ray tracing
	for (int s = 0; s < ns; ++s) {
		for (int j = 0; j < ny; ++j) {
			for (int i = 0; i < nx; ++i) {
				
				// initialize two random variables, drand48() range: [0.0, 1.0)
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);

				ray r = cam.get_ray(u, v);
				vec3 radiance = color(r, world, 0);

				image[(j * nx + i) * 3 + 0] += radiance.x();
				image[(j * nx + i) * 3 + 1] += radiance.y();
				image[(j * nx + i) * 3 + 2] += radiance.z();
			}
		}

		// Save current result
		std::string outputFilepath = "output/output_" + std::to_string(s + 1) + ".png";
		SaveImage(outputFilepath.c_str(), image, nx, ny, s);
		std::cout << "Saved: " << outputFilepath << std::endl;
	}

	delete image;
}