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

vec3 RED = vec3(1.0, 0.0, 0.0);

void main(void)
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(fNormal);
	vec3 light_dir = normalize(light.position - fPos);

	// Diffuse
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuse;

	// Specular
	vec3 view_dir = normalize(eye - fPos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	// vec3 reflect_dir = normalize(2*dot(light_dir,norm)*norm-light_dir); // Half vector
	// Calculating the reflection direction for each fragment is heavy, so we can use half vector (Blinn-Phong Lighting Model)
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = (spec * material.specular) * light.specular;

	vec3 result = (ambient + diffuse + specular);

	out_color = vec4(result, 1.0);
}
