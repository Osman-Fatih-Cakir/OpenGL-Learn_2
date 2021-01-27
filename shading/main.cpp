
/**
	
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include <shaders.h>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

int WIDTH = 800;
int HEIGHT = 800;

// Lighting 
vec3 light_pos = vec3(0.f, 3.f, 0.f);
vec3 light_ambient = vec3(0.2f, 0.2f, 0.2f);
vec3 light_diffuse = vec3(0.5f, 0.5f, 0.5f);
vec3 light_specular = vec3(1.0f, 1.0f, 1.0f);

vec3 ambient = vec3(1.0f, 0.5f, 0.3f);
vec3 diffuse = vec3(1.0f, 0.5f, 0.3f);
vec3 specular = vec3(0.5f, 0.5f, 0.5f);
float shininess = 32.0f;

GLuint light_pos_loc, light_amb_loc, light_dif_loc, light_spe_loc;
GLuint light_col_loc;
GLuint eye_loc, ambient_loc, diffuse_loc, specular_loc, shininess_loc;

vec3 eye; vec3 up;
mat4 projection, modelview, normal_matrix;

GLuint vertex_shader, fragment_shader, shader_program;
GLuint projection_loc, modelview_loc, normal_matrix_loc;

GLuint modelVAO, modelVBO, modelNBO, modelEBO;
std::vector <glm::vec3> model_vertices;
std::vector <glm::vec3> model_normals;
std::vector <unsigned int> model_indices;

// Cube attributes
GLfloat cube_vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
};
GLfloat cube_normals[] = {
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,

	  0.0f,  0.0f,  1.0f,
	  0.0f,  0.0f,  1.0f,
	  0.0f,  0.0f,  1.0f,
	  0.0f,  0.0f,  1.0f,
	  0.0f,  0.0f,  1.0f,
	  0.0f,  0.0f,  1.0f,

	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,

	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f
};
GLuint cubeVAO;

void init();
void changeViewport(int, int);
void keyboard(unsigned char key, int x, int y);
void parse_teapot(const char*);
void init_teapot();
void draw_teapot();
void init_cube();
void draw_cube();
void render();

// Main function
int main(int argc, char* argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// Create window
	glutCreateWindow("OPENGL APP");
	glEnable(GL_DEPTH_TEST);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutReshapeWindow(WIDTH, HEIGHT);

	// Bind functions
	glutReshapeFunc(changeViewport);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);

	// Initialize Glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Unable to initalize Glew ! " << std::endl;
		return 1;
	}

	// Start program
	init();

	glutMainLoop();

	return 0;
}

void changeViewport(int w, int h)
{
	glViewport(0, 0, w, h);
}

// Initialize the scene
void init()
{
	// Initialize viewing values
	eye = vec3(0.f, 0.f, 3.f);
	up = vec3(0.f, 1.f, 0.f);
	projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);
	modelview = glm::lookAt(eye, vec3(0.0f, 0.0f, 0.0f), up);
	normal_matrix = glm::inverse(glm::transpose(modelview));

	// Initialize shaders
	vertex_shader = initshaders(GL_VERTEX_SHADER, "shaders/vertex_shader.glsl");
	fragment_shader = initshaders(GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl");
	shader_program = initprogram(vertex_shader, fragment_shader);

	// Get the locations of other uniform variables
	projection_loc = glGetUniformLocation(shader_program, "projection");
	modelview_loc = glGetUniformLocation(shader_program, "modelview");
	normal_matrix_loc = glGetUniformLocation(shader_program, "normal_matrix");
	// Light uniforms
	eye_loc = glGetUniformLocation(shader_program, "eye_pos");
	ambient_loc = glGetUniformLocation(shader_program, "material.ambient");
	diffuse_loc = glGetUniformLocation(shader_program, "material.diffuse");
	specular_loc = glGetUniformLocation(shader_program, "material.specular");
	shininess_loc = glGetUniformLocation(shader_program, "material.shininess");
	light_pos_loc = glGetUniformLocation(shader_program, "light.position");
	light_amb_loc = glGetUniformLocation(shader_program, "light.ambient");
	light_dif_loc = glGetUniformLocation(shader_program, "light.diffuse");
	light_spe_loc = glGetUniformLocation(shader_program, "light.specular");

	parse_teapot("teapot.obj");

	init_teapot();
	init_cube();
}

// Keyboard inputs
void keyboard(unsigned char key, int x, int y)
{

}

// OBJ file parser function. Used for loading the teapot.obj file.
void parse_teapot(const char* filepath)
{
	FILE* fp;
	float x, y, z;
	int fx, fy, fz, ignore;
	int c1, c2;
	float minY = INFINITY, minZ = INFINITY;
	float maxY = -INFINITY, maxZ = -INFINITY;

	fopen_s(&fp, filepath, "rb");
	if (fp == NULL) // File open failed
	{
		std::cerr << "Error loading file: " << filepath << std::endl;
		std::getchar();
		exit(-1);
	}

	while (!feof(fp))
	{
		c1 = fgetc(fp);

		while (!(c1 == 'v' || c1 == 'f'))
		{
			c1 = fgetc(fp);
			if (feof(fp))
				break;
		}
		c2 = fgetc(fp);

		if ((c1 == 'v') && (c2 == ' '))
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			model_vertices.push_back(glm::vec3(x, y, z));

			// Taking maximum and minimum values for center the object after loading
			if (y < minY) minY = y;
			if (z < minZ) minZ = z;
			if (y > maxY) maxY = y;
			if (z > maxZ) maxZ = z;
		}
		else if ((c1 == 'v') && (c2 == 'n'))
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			model_normals.push_back(glm::normalize(glm::vec3(x, y, z)));
		}
		else if (c1 == 'f')
		{
			fscanf_s(fp, "%d//%d %d//%d %d//%d", &fx, &ignore, &fy, &ignore, &fz, &ignore);
			model_indices.push_back(fx - 1);
			model_indices.push_back(fy - 1);
			model_indices.push_back(fz - 1);
		}
	}

	fclose(fp); // Done parsing

	// Center the teapot
	float avgY = (minY + maxY) / 2.0f - 0.0234f;
	float avgZ = (minZ + maxZ) / 2.0f;
	for (unsigned int i = 0; i < model_vertices.size(); i++)
	{
		glm::vec3 shifted_vertex = (model_vertices[i] - glm::vec3(0.0f, avgY, avgZ));
		model_vertices[i] = shifted_vertex;
	}
}

// Initialize model
void init_teapot()
{
	glGenVertexArrays(1, &modelVAO);
	glBindVertexArray(modelVAO);
	// Vertices
	glGenBuffers(1, &modelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model_vertices.size(), &model_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // Use layout 0 for vertices in vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	
	// Normals
	glGenBuffers(1, &modelNBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelNBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model_normals.size(), &model_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Indices
	glGenBuffers(1, &modelEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * model_indices.size(), &model_indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); // Prevent further modifications
}

// Initalize cube model
void init_cube()
{
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	// Vertices
	GLuint VBO, NBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // Use layout 0 for vertices in vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); // Prevent further modifications
}

// Draw teapot with elements
void draw_teapot()
{
	glBindVertexArray(modelVAO);
	glDrawElements(GL_TRIANGLES, model_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0); // Prevent further modifications
}

// Draw cube
void draw_cube()
{
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); // Prevent further modifications
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5, 0.5, 0, 1);

	// Send camera uniforms
	glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &(modelview)[0][0]);
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &(projection)[0][0]);
	glUniformMatrix4fv(normal_matrix_loc, 1, GL_FALSE, &(normal_matrix)[0][0]);

	// Send light variables to shader
	glUniform3fv(eye_loc, 1, &eye[0]);
	glUniform3fv(ambient_loc, 1, &ambient[0]);
	glUniform3fv(diffuse_loc, 1, &diffuse[0]);
	glUniform3fv(specular_loc, 1, &specular[0]);
	glUniform1f(shininess_loc, shininess);
	glUniform3fv(light_pos_loc, 1, &light_pos[0]);
	glUniform3fv(light_amb_loc, 1, &light_ambient[0]);
	glUniform3fv(light_dif_loc, 1, &light_diffuse[0]);
	glUniform3fv(light_spe_loc, 1, &light_specular[0]);

	draw_teapot();
	//draw_cube();

	glutSwapBuffers();
}