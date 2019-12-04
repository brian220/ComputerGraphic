#ifndef MATERIALH
#define MATERIALH

#include "ray.hpp"
#include "hitable.hpp"
#include "texture.hpp"
#include "random.hpp"

vec3 reflect(const vec3 v, const vec3 n);

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted);

vec3 random_in_unit_sphere(void);

class material {
    public:
        virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
		virtual vec3 emitted() const;
};

class lambertian : public material {
    public:
		// lambertian with texture
        lambertian(texture* tex) : tex_ptr(tex) {}
		// lambertian with color only
		// lambertian(const vec3& a) :tex_ptr(new constant_texture(a)) {}
		lambertian(const vec3& a) : albedo(a) {}

		~lambertian() { delete tex_ptr; }
		
		virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override;

		vec3 albedo;
		texture* tex_ptr;
};

class metal : public material {
    public:
        metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1;}
		metal(const vec3& a) : albedo(a) {}
		virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override;

        vec3 albedo;
        float fuzz;
};

class dielectric : public material {
	public:
		dielectric(float ri) : ref_idx(ri) {}
		virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override;

		float ref_idx;
};

class light : public material {
	public:
		light(const vec3& c) : emit(c) {}
		virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override;
		virtual vec3 emitted() const override;

		vec3 emit;
};

#endif