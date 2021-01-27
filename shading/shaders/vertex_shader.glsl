#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vNormal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normal_matrix;

uniform vec3 translation;
uniform vec3 rotation;

out vec3 fNormal;
out vec3 fPos;

void main()
{
	gl_Position = projection * modelview * vec4(position, 1.0f);

	fNormal = mat3(normal_matrix) * vNormal;  // Multiply with normal matrix (in case of non-uniform scaling)
	fPos = vec3(modelview * vec4(position, 1.0f)); 
	fNormal = vNormal;
}