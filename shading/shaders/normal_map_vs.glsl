#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vTex_coord;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 normal_matrix;

out vec3 fPos;
out vec2 fTex_coord;
out vec3 fNormal;
out mat3 TBN;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);

	// Create tangent matrix
	vec3 T = normalize(vec3(model * vec4(vTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(vNormal, 0.0)));
	vec3 B = normalize(vec3(model * vec4(vBitangent, 0.0)));
	//T = normalize(T - dot(T, N) * N);
	//vec3 B = normalize(cross(N, T));
	TBN = mat3(T, B, N);

	fPos = vec3(model * vec4(position, 1.0f));
    fTex_coord = vTex_coord;
	fNormal = vec3(normal_matrix * vec4(vNormal, 1.0));
}
