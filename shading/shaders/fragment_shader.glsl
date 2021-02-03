#version 330 core

in vec3 fNormal;
in vec3 fPos;

uniform vec3 eye;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

out vec4 out_color;

void main(void)
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(fNormal);
	vec3 light_dir = normalize(light.position - fPos);
	vec3 view_dir = normalize(eye - fPos);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	// Calculating the reflection direction for each fragment is heavy, so we can use half vector (Blinn-Phong Lighting Model)

	// Diffuse
	float diff = max(dot(norm, light_dir), 0.0);
	diffuse = (diff * material.diffuse) * light.diffuse;

	// Specular
	vec3 reflect_dir = normalize(view_dir + light_dir); // Half vector
	float spec = pow(max(dot(norm, reflect_dir), 0.0), material.shininess);
	specular = (spec * material.specular) * light.specular;

	vec3 result = (ambient + diffuse + specular);

	out_color = vec4(result, 1.0);
}
