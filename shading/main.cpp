
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

bool is_blur = false;
GLuint blur_loc;

vec3 eye = vec3(0.f, 0.f, 3.f);
vec3 up = vec3(0.f, 1.f, 0.f);

// Lighting 
vec3 light_pos = vec3(0.f, 0.f, 3.f);
vec3 light_ambient = vec3(0.4f, 0.3f, 0.1f);
vec3 light_diffuse = vec3(0.5f, 0.5f, 0.5f);
vec3 light_specular = vec3(0.8f, 0.6f, 0.4f);

vec3 ambient = vec3(0.4f, 0.2f, 0.14f);
vec3 diffuse = vec3(1.0f, 0.5f, 0.3f);
vec3 specular = vec3(0.7f, 0.7f, 0.7f);
float shininess = 64.0f;

GLuint light_pos_loc, light_amb_loc, light_dif_loc, light_spe_loc;
GLuint light_col_loc;
GLuint eye_loc, ambient_loc, diffuse_loc, specular_loc, shininess_loc;

mat4 projection, modelview, normal_matrix;

GLuint vertex_shader, fragment_shader, shader_program, screen_shader_program;
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

bool is_teapot = true;

void init();
void changeViewport(int, int);
void keyboard(unsigned char key, int x, int y);
void parse_teapot(const char*);
void init_screen_shaders();
void init_framebuffer();
void init_teapot();
void draw_teapot();
void init_cube();
void draw_cube();
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
			glEnable(GL_MULTISAMPLE);
			std::cout << "Multisampled AA activated" << std::endl;
			break;
		case 'c':
			glDisable(GL_MULTISAMPLE);
			std::cout << "Multisampled AA deactivated" << std::endl;
			break;
		case 'e':
			is_blur = !is_blur;
			if (is_blur) std::cout << "Blur activated." << std::endl;
			else std::cout << "Blur deactivated." << std::endl;
			break;
	}
	modelview = glm::lookAt(eye, vec3(0.f, 0.f, 0.f), up);
	glutPostRedisplay();
}

// Initialize the scene
void init()
{
	std::cout << "W/S/A/D/R/F to rotate the camera." << std::endl;
	std::cout << "Q to render cube or teapot." << std::endl;
	std::cout << "X/C to toggle msaa." << std::endl;

	// Initialize viewing values
	projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);
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

	parse_teapot("teapot.obj");
	init_screen_shaders();
	init_framebuffer();
	init_teapot();
	init_cube();
}

// Initialize post-process shaders
void init_screen_shaders()
{
	// Initialize shaders
	GLuint vertex_shader = initshaders(GL_VERTEX_SHADER, "shaders/screen_vs.glsl");
	GLuint fragment_shader = initshaders(GL_FRAGMENT_SHADER, "shaders/screen_fs.glsl");
	screen_shader_program = initprogram(vertex_shader, fragment_shader);
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

void draw_quad()
{
	GLuint texture_loc = glGetUniformLocation(screen_shader_program, "screen_texture");
	//glUniform1i(texture_loc, 0);
	//glActiveTexture(GL_TEXTURE0);
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5, 0.5, 0, 1);

	glUseProgram(shader_program);

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

	if (is_teapot)
		draw_teapot();
	else
		draw_cube();

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
}
