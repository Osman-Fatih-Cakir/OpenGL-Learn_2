#version 330 core

in vec3 fPos;
in vec2 fTex_coord;
in vec3 fNormal;
in mat3 TBN;

uniform vec3 eye;
uniform vec3 light_pos;
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;
uniform sampler2D image;
uniform sampler2D normal_map;
uniform int is_normal_map;

out vec4 out_color;

void main(void)
{
    vec3 color = texture(image, fTex_coord).rgb; // Texture color

	vec3 ambient =  material.ambient;

	vec3 norm;
	if (is_normal_map == 1)
	{
		// Normal map
		norm = texture(normal_map, fTex_coord).rgb;
		// Normalize normal map (set range [0:1] to [-1:1])
		norm = normalize(2.0 * norm - 1.0);
		norm = normalize(TBN * norm);
	}
	else
	{
		norm = normalize(fNormal);
	}

	vec3 light_dir = normalize(light_pos - fPos);
	vec3 view_dir = normalize(eye - fPos);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	// Calculating the reflection direction for each fragment is heavy, 
	// so we can use half vector (Blinn-Phong Lighting Model)

	// Diffuse
	float diff = max(dot(norm, light_dir), 0.0);
	diffuse = (diff * material.diffuse);

	// Specular
	vec3 reflect_dir = normalize(view_dir + light_dir); // Half vector
	float spec = pow(max(dot(norm, reflect_dir), 0.0), material.shininess);
	specular = (spec * material.specular);

	vec3 result = color * (ambient + diffuse + specular);

	out_color = vec4(result, 1.0);
}
