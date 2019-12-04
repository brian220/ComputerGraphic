#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

image_texture::image_texture(const char* filepath)
{
	// write something here ...

	// Hint: load image to variable "data"
}

vec3 image_texture::value_at_uv(float _u, float _v, const vec3& _hit_point) const
{
	// write something here ...

	return vec3(0, 0, 0);
}

vec3 constant_texture::value_at_uv(float _u, float _v, const vec3& _hit_point) const
{
	return color;
}