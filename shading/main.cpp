
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> // Image loading library for load texture images

#include <shaders.h>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

int WIDTH = 800;
int HEIGHT = 800;

GLuint ms_framebuffer, in_framebuffer, quadVAO, texture_colorbuffer, screen_texture;
GLfloat quad_vertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

vec3 eye = vec3(0.f, 0.f, 3.f);
vec3 up = vec3(0.f, 1.f, -1.f);

// Lighting 
vec3 light_pos = vec3(4.f, 4.f, 1.f);
vec3 light_ambient = vec3(0.2f, 0.2f, 0.2f);
vec3 light_diffuse = vec3(0.8f, 0.8f, 0.8f);
vec3 light_specular = vec3(1.f, 1.f, 1.f);

vec3 ambient = vec3(0.01f, 0.01f, 0.01f);
vec3 diffuse = vec3(0.8f, 0.8f, 0.8f);
vec3 specular = vec3(0.9f, 0.9f, 0.9f);
float shininess = 64.0f;

GLuint light_pos_loc, light_amb_loc, light_dif_loc, light_spe_loc;
GLuint light_col_loc;
GLuint eye_loc, ambient_loc, diffuse_loc, specular_loc, shininess_loc;

mat4 projection, modelview, normal_matrix;

GLuint n_eye_loc, n_ambient_loc, n_diffuse_loc, n_specular_loc, n_shininess_loc;
GLuint n_projection_loc, n_model_loc, n_view_loc, n_light_pos_loc, n_texture_loc, n_normal_map_loc;
GLuint n_normal_matrix;

GLuint vertex_shader, fragment_shader, shader_program, screen_shader_program, normal_map_shader_program;
GLuint projection_loc, modelview_loc, normal_matrix_loc;

GLuint modelVAO, modelVBO, modelNBO, modelEBO;
std::vector <glm::vec3> model_vertices;
std::vector <glm::vec3> model_normals;
std::vector <unsigned int> model_indices;

// Floor attributes
GLfloat floor_vertices[] = {
	-1.0f,  1.0f, 0.f,
	-1.0f, -1.0f, 0.f,
	 1.0f, -1.0f, 0.f,

	-1.0f,  1.0f, 0.f,
	 1.0f, -1.0f, 0.f,
	 1.0f,  1.0f, 0.f
};
GLfloat floor_tex_coord[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};
GLfloat floor_normals[] = {
	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f,

	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f
};
mat4 floor_model;
GLuint floorVAO;

GLuint texture, normal_texture;

bool active_multisample_aa = true;
bool is_blur = false;
bool is_teapot = true;
bool is_normal_map = true;
GLuint blur_loc, is_normal_map_loc;

void init();
void changeViewport(int, int);
void keyboard(unsigned char key, int x, int y);
GLuint load_texture(const char* path);
void parse_teapot(const char*);
void init_screen_shaders();
void init_normal_map_shaders();
void init_normal_map_shader_uniforms();
void init_framebuffer();
void init_teapot();
void draw_teapot();
void init_floor();
void init_floor_tangent_space();
void set_shader_program_uniforms();
void set_normal_map_shader_uniforms();
void draw_quad();
void render();

// Main function
int main(int argc, char* argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

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

// Keyboard inputs
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			eye = vec3(glm::rotate(mat4(1.f), glm::radians(15.f), vec3(1.f, 0.f, 0.f)) * vec4(eye, 1.f));
			up = vec3(glm::rotate(mat4(1.f), glm::radians(15.f), vec3(1.f, 0.f, 0.f)) * vec4(up, 1.f));
			break;
		case 's':
			eye = vec3(glm::rotate(mat4(1.f), glm::radians(-10.f), vec3(1.f, 0.f, 0.f)) * vec4(eye, 1.f));
			up = vec3(glm::rotate(mat4(1.f), glm::radians(-10.f), vec3(1.f, 0.f, 0.f)) * vec4(up, 1.f));
			break;
		case 'a':
			eye = vec3(glm::rotate(mat4(1.f), glm::radians(10.f), vec3(0.f, 1.f, 0.f)) * vec4(eye, 1.f));
			up = vec3(glm::rotate(mat4(1.f), glm::radians(10.f), vec3(0.f, 1.f, 0.f)) * vec4(up, 1.f));
			break;
		case 'd':
			eye = vec3(glm::rotate(mat4(1.f), glm::radians(-10.f), vec3(0.f, 1.f, 0.f)) * vec4(eye, 1.f));
			up = vec3(glm::rotate(mat4(1.f), glm::radians(-10.f), vec3(0.f, 1.f, 0.f)) * vec4(up, 1.f));
			break;
		case 'r':
			eye = vec3(glm::rotate(mat4(1.f), glm::radians(10.f), vec3(0.f, 0.f, 1.f)) * vec4(eye, 1.f));
			up = vec3(glm::rotate(mat4(1.f), glm::radians(10.f), vec3(0.f, 0.f, 1.f)) * vec4(up, 1.f));
			break;
		case 'f':
			eye = vec3(glm::rotate(mat4(1.f), glm::radians(-10.f), vec3(0.f, 0.f, 1.f)) * vec4(eye, 1.f));
			up = vec3(glm::rotate(mat4(1.f), glm::radians(-10.f), vec3(0.f, 0.f, 1.f)) * vec4(up, 1.f));
			break;
		case 'q':
			is_teapot = !is_teapot;
			break;
		case 'x':
			active_multisample_aa = !active_multisample_aa;
			if (active_multisample_aa)
			{
				glEnable(GL_MULTISAMPLE);
				std::cout << "Multisampled AA activated" << std::endl;
			}
			else
			{
				glDisable(GL_MULTISAMPLE);
				std::cout << "Multisampled AA deactivated" << std::endl;
			}
			break;
		case 'e':
			is_blur = !is_blur;
			if (is_blur) std::cout << "Blur activated." << std::endl;
			else std::cout << "Blur deactivated." << std::endl;
			break;
		case 'z':
			is_normal_map = !is_normal_map;
			break;
		case 'y':
			floor_model = glm::rotate(floor_model, glm::radians(10.0f), vec3(-1.f, 0.f, 0.f));
			break;
		case 'h':
			floor_model = glm::rotate(floor_model, glm::radians(10.0f), vec3(1.f, 0.f, 0.f));
			break;
		case 'g':
			floor_model = glm::rotate(floor_model, glm::radians(10.0f), vec3(0.f, -1.f, 0.f));
			break;
		case 'j':
			floor_model = glm::rotate(floor_model, glm::radians(10.0f), vec3(0.f, 1.f, 0.f));
			break;
	}
	modelview = glm::lookAt(eye, vec3(0.f, 0.f, 0.f), up);
	glutPostRedisplay();
}

// Initialize the scene
void init()
{
	std::cout << "'W/S/A/D/R/F' to rotate the camera." << std::endl;
	std::cout << "'E' to see blur." << std::endl;
	std::cout << "'Q' to render wall or teapot." << std::endl;
	std::cout << "'X' to toggle msaa." << std::endl;
	std::cout << "'Z' to toggle normal mapping on wall." << std::endl;
	std::cout << "'Y/H/G/J' to rotate the wall.'" << std::endl;

	// Initialize viewing values
	projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
	modelview = glm::lookAt(eye, vec3(0.0f, 0.0f, 0.0f), up);
	normal_matrix = glm::inverse(glm::transpose(modelview));

	// Initialize shaders
	vertex_shader = initshaders(GL_VERTEX_SHADER, "shaders/vertex_shader.glsl");
	fragment_shader = initshaders(GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl");
	shader_program = initprogram(vertex_shader, fragment_shader);

	// Initialize post-process shaders
	init_screen_shaders();

	// Get the locations of other uniform variables
	projection_loc = glGetUniformLocation(shader_program, "projection");
	modelview_loc = glGetUniformLocation(shader_program, "modelview");
	normal_matrix_loc = glGetUniformLocation(shader_program, "normal_matrix");
	// Light uniforms
	eye_loc = glGetUniformLocation(shader_program, "eye");
	ambient_loc = glGetUniformLocation(shader_program, "material.ambient");
	diffuse_loc = glGetUniformLocation(shader_program, "material.diffuse");
	specular_loc = glGetUniformLocation(shader_program, "material.specular");
	shininess_loc = glGetUniformLocation(shader_program, "material.shininess");
	light_pos_loc = glGetUniformLocation(shader_program, "light.position");
	light_amb_loc = glGetUniformLocation(shader_program, "light.ambient");
	light_dif_loc = glGetUniformLocation(shader_program, "light.diffuse");
	light_spe_loc = glGetUniformLocation(shader_program, "light.specular");

	blur_loc = glGetUniformLocation(screen_shader_program, "is_blur");

	// Load textures
	texture = load_texture("brickwall.jpg");
	normal_texture = load_texture("brickwall_normal.jpg");

	parse_teapot("teapot.obj");
	init_screen_shaders();
	init_normal_map_shaders();
	init_normal_map_shader_uniforms();
	init_floor();
	init_framebuffer();
	init_teapot();
}

// Initialize post-process shaders
void init_screen_shaders()
{
	// Initialize shaders
	GLuint vertex_shader = initshaders(GL_VERTEX_SHADER, "shaders/screen_vs.glsl");
	GLuint fragment_shader = initshaders(GL_FRAGMENT_SHADER, "shaders/screen_fs.glsl");
	screen_shader_program = initprogram(vertex_shader, fragment_shader);
}

// Initialize normal map shaders
void init_normal_map_shaders()
{
	// Initialize shaders
	GLuint vertex_shader = initshaders(GL_VERTEX_SHADER, "shaders/normal_map_vs.glsl");
	GLuint fragment_shader = initshaders(GL_FRAGMENT_SHADER, "shaders/normal_map_fs.glsl");
	normal_map_shader_program = initprogram(vertex_shader, fragment_shader);
}

// Get locations of uniforms
void init_normal_map_shader_uniforms()
{
	// Get the locations of other uniform variables
	n_projection_loc = glGetUniformLocation(normal_map_shader_program, "projection");
	n_model_loc = glGetUniformLocation(normal_map_shader_program, "model");
	n_view_loc = glGetUniformLocation(normal_map_shader_program, "view");
	n_normal_matrix = glGetUniformLocation(normal_map_shader_program, "normal_matrix");

	// Light uniforms
	n_eye_loc = glGetUniformLocation(normal_map_shader_program, "eye");
	n_light_pos_loc = glGetUniformLocation(normal_map_shader_program, "light_pos");
	n_ambient_loc = glGetUniformLocation(normal_map_shader_program, "material.ambient");
	n_diffuse_loc = glGetUniformLocation(normal_map_shader_program, "material.diffuse");
	n_specular_loc = glGetUniformLocation(normal_map_shader_program, "material.specular");
	n_shininess_loc = glGetUniformLocation(normal_map_shader_program, "material.shininess");

	n_texture_loc = glGetUniformLocation(normal_map_shader_program, "image");
	n_normal_map_loc = glGetUniformLocation(normal_map_shader_program, "normal_map");

	is_normal_map_loc = glGetUniformLocation(normal_map_shader_program, "is_normal_map");
}

// Loads image and returns texture id
GLuint load_texture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* image = stbi_load(path, &width, &height, &nrComponents, 0);
	if (image)
	{
		GLenum format;
		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	return textureID;
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

void init_framebuffer()
{
	//
	//// Multisampled framebuffer
	//

	glGenFramebuffers(1, &ms_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ms_framebuffer);

	// Create color attachment texture
	glGenTextures(1, &texture_colorbuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_colorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WIDTH, HEIGHT, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_colorbuffer, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// Create renderbuffer object attachment for depth and stencil buffers
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Check is buffer complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create a VAO for screen texture
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 6, quad_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//
	//// Post-processing frame buffer
	//

	glGenFramebuffers(1, &in_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, in_framebuffer);
	
	// create a color attachment texture
	glGenTextures(1, &screen_texture);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);	// we only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Intermediate framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Init floor attributes
void init_floor()
{
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);
	// Vertices
	GLuint VBO, TBO, NBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // Use layout 0 for vertices in vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture coordinates
	glGenBuffers(1, &TBO);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_tex_coord), floor_tex_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // Use layout 1 for texture coordinates in vertex shader
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_normals), floor_normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); // Use layout 2 for normals in vertex shader
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0); // Prevent further modifications

	// Model matrix of floor
	floor_model = glm::scale(mat4(1.f), vec3(2.f, 2.f, 2.f));
	floor_model = glm::translate(floor_model, vec3(0.f, 0.f, -0.5f));

	init_floor_tangent_space();
}


// Initialize a tangent space for each triangle
void init_floor_tangent_space()
{
	// We initalize the tangent vectors manually
	vec3 tangent1, tangent2, bitangent1, bitangent2;

	// Triangle 1
	vec3 edge1 = vec3(floor_vertices[3], floor_vertices[4], floor_vertices[5]) - vec3(floor_vertices[0], floor_vertices[1], floor_vertices[2]);
	vec3 edge2 = vec3(floor_vertices[6], floor_vertices[7], floor_vertices[8]) - vec3(floor_vertices[0], floor_vertices[1], floor_vertices[2]);
	vec2 deltaUV1 = vec2(floor_tex_coord[2], floor_tex_coord[3]) - vec2(floor_tex_coord[0], floor_tex_coord[1]);
	vec2 deltaUV2 = vec2(floor_tex_coord[4], floor_tex_coord[5]) - vec2(floor_tex_coord[0], floor_tex_coord[1]);

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	// Triangle 2
	edge1 = vec3(floor_vertices[6], floor_vertices[7], floor_vertices[8]) - vec3(floor_vertices[0], floor_vertices[1], floor_vertices[0]);
	edge2 = vec3(floor_vertices[12], floor_vertices[13], floor_vertices[14]) - vec3(floor_vertices[0], floor_vertices[1], floor_vertices[2]);-
	deltaUV1 = vec2(floor_tex_coord[4], floor_tex_coord[5]) - vec2(floor_tex_coord[0], floor_tex_coord[1]);
	deltaUV2 = vec2(floor_tex_coord[8], floor_tex_coord[9]) - vec2(floor_tex_coord[0], floor_tex_coord[1]);

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	GLfloat tangents[] = {
		tangent1.x, tangent1.y, tangent1.z,
		tangent1.x, tangent1.y, tangent1.z,
		tangent1.x, tangent1.y, tangent1.z,

		tangent2.x, tangent2.y, tangent2.z,
		tangent2.x, tangent2.y, tangent2.z,
		tangent2.x, tangent2.y, tangent2.z
	};
	
	GLfloat bitangents[] = {
		bitangent1.x, bitangent1.y, bitangent1.z,
		bitangent1.x, bitangent1.y, bitangent1.z,
		bitangent1.x, bitangent1.y, bitangent1.z,

		bitangent2.x, bitangent2.y, bitangent2.z,
		bitangent2.x, bitangent2.y, bitangent2.z,
		bitangent2.x, bitangent2.y, bitangent2.z
	};

	// Bind the tangent variables to buffers
	//glBindVertexArray(floorVAO);
	// Tangent vectors
	GLuint TBO, BBO;
	glGenBuffers(1, &TBO);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3); // Use layout 3 for tangent vectors in vertex shader
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Bitangent vectors
	glGenBuffers(1, &BBO);
	glBindBuffer(GL_ARRAY_BUFFER, BBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents), bitangents, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4); // Use layout 4 for bitangent vectors
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); // Prevent further modifications
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


// Setting uniforms of the shader
void set_shader_program_uniforms()
{
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
}

// Draw teapot with elements
void draw_teapot()
{
	glBindVertexArray(modelVAO);
	glDrawElements(GL_TRIANGLES, model_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0); // Prevent further modifications
}

// Set uniforms in normal_map shader
void set_normal_map_shader_uniforms()
{
	mat4 floor_normal_matrix = glm::inverse(glm::transpose(floor_model));
	
	// Send camera uniforms
	glUniformMatrix4fv(n_view_loc, 1, GL_FALSE, &(modelview)[0][0]); // "modelview" varaible is actually view variable
	glUniformMatrix4fv(n_model_loc, 1, GL_FALSE, &(floor_model)[0][0]);
	glUniformMatrix4fv(n_projection_loc, 1, GL_FALSE, &(projection)[0][0]);
	glUniformMatrix4fv(n_normal_matrix, 1, GL_FALSE, &(floor_normal_matrix)[0][0]);

	// Send light variables to shader
	glUniform3fv(n_eye_loc, 1, &eye[0]);
	glUniform3fv(n_ambient_loc, 1, &ambient[0]);
	glUniform3fv(n_diffuse_loc, 1, &diffuse[0]);
	glUniform3fv(n_specular_loc, 1, &specular[0]);
	glUniform1f(n_shininess_loc, shininess);
	glUniform3fv(n_light_pos_loc, 1, & light_pos[0]);

	glUniform1i(is_normal_map_loc, (int)is_normal_map);
}

// Draw floor
void draw_floor()
{
	glBindVertexArray(floorVAO);

	// Textures
	glUniform1i(n_texture_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(n_normal_map_loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_texture);

	// Draw floor
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0); // Prevent further modification
}

void draw_quad()
{
	GLuint texture_loc = glGetUniformLocation(screen_shader_program, "screen_texture");
	glUniform1i(texture_loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_texture);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void render()
{
	// Bind framebuffer and draw the scene as we normally would to color texture
	glBindFramebuffer(GL_FRAMEBUFFER, ms_framebuffer);
	glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5, 0.5, 0, 1);
	
	if (is_teapot) // Draw teapot
	{
		glUseProgram(shader_program);
		set_shader_program_uniforms();
		draw_teapot();
	}
	else // Draw normal mapped wall
	{
		glUseProgram(normal_map_shader_program);
		set_normal_map_shader_uniforms();
		draw_floor();
	}
	
	// Blit multisampled buffer(s) to normal colorbuffer of intermediate framebuffer. Image is stored in screen_texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ms_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, in_framebuffer);
	glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // Disable depth test so screen-space quad isn't discarded due to depth test.
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(screen_shader_program); // Activate post-process shaders
	
	glUniform1i(blur_loc, (int)is_blur); // Toogle blur

	draw_quad(); // Draw screen quad (textured)
	
	glutSwapBuffers();

	// Error catch
	GLuint err = glGetError();
	if (err) fprintf(stderr, "%s\n", gluErrorString(err));

	glutPostRedisplay(); // Render loop
}
