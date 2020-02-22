#include "reflection.h"

Reflection::Reflection(short width, short height) :ShadowRays(width, height)
{
}

Reflection::~Reflection()
{
}

Payload Reflection::Hit(const Ray& ray, const IntersectableData& data, const MaterialTriangle* triangle, const unsigned int max_raytrace_depth) const
{
	if (triangle == nullptr)
	{
		return Miss(ray);
	}
	Payload payload;
	payload.color = triangle->emissive_color;

	float3 X = ray.position + ray.direction * data.t;
	float3 N = triangle->GetNormal(data.baricentric);

	if (triangle->reflectiveness)
	{
		Ray reflection_ray(X, ray.direction - 2.f * dot(N, ray.direction) * N);
		return TraceRay(reflection_ray, max_raytrace_depth - 1);
	}

	for (auto& light : lights)
	{
		Ray toLight(X, light->position - X);
		float toLightDistance = length(light->position - X);
		float t = TraceShadowRay(toLight, toLightDistance);
		if (fabs(t - toLightDistance) > t_min)
		{
			continue;
		}

		// Diffuse
		payload.color += light->color * triangle->diffuse_color
			* std::max(dot(N, toLight.direction), 0.f);

		// Specular
		float3 reflectionDirection = 2.f * dot(N, toLight.direction) * N - toLight.direction;
		payload.color += light->color * triangle->specular_color
			* powf(std::max(dot(ray.direction, reflectionDirection), 0.f), triangle->specular_exponent);
	}
	return payload;
}
