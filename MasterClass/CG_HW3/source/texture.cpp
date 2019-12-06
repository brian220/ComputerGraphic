#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

image_texture::image_texture(const char* filepath)
{   
	// Hint: load image to variable "data"
	data = stbi_load(filepath, &width, &height, &channel, 0);
}

vec3 image_texture::value_at_uv(float _u, float _v, const vec3& _hit_point) const
{
	int i = (_u) * width;
	int j = (1 - _v) * height - 0.001;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > width - 1) i = width - 1;
	if (j > height - 1) j = height - 1;
	float r = int(data[3 * i + 3 * width * j]) / 255.0;
	float g = int(data[3 * i + 3 * width * j + 1]) / 255.0;
	float b = int(data[3 * i + 3 * width * j + 2]) / 255.0;
	return vec3(r, g, b);
}

vec3 constant_texture::value_at_uv(float _u, float _v, const vec3& _hit_point) const
{
	return color;
}