#include "glew.h"
#include "glfw3.h"
#include "glm.hpp"

#include "matrix_transform.hpp"
#include "type_ptr.hpp"

#include <vector>
#include <string>
#include <fstream>
#include "GLSLProgram.h"

glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 5.0f);

glm::mat4 triangle_model_matrix;
glm::mat4 view_matrix;
glm::mat4 projection_matrix;

const GLuint DEFAULT_WINDOW_WIDTH = 800, DEFAULT_WINDOW_HEIGHT = 800;
const GLfloat CAMERA_MOVEMENT_SPEED = 0.02f;

GLSLProgram* shaderProgram;

double ypos_old = -1;

void framebuffer_size_callback(GLFWwindow* window, int new_screen_width, int new_screen_height)
{
	glViewport(0, 0, new_screen_width, new_screen_height);
	projection_matrix = glm::perspective(45.0f, (GLfloat)new_screen_width / (GLfloat)new_screen_height, 0.1f, 100.0f);
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if(ypos_old != -1)
		{
			camera_position.z += CAMERA_MOVEMENT_SPEED * (ypos_old - ypos);
			camera_position.z = glm::clamp(camera_position.z, 0.0f, 10.0f);	
		}

		ypos_old = ypos;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		ypos_old = -1;
	}

}

void cleanUp()
{
	delete shaderProgram;
}

int main()
{

	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		return -1;
	}

	shaderProgram = new GLSLProgram();

	if(!shaderProgram->compileShaderFromFile("triangle.vs", GL_VERTEX_SHADER))
	{
		printf("Vertex shader failed to compile!\n%s", shaderProgram->log().c_str());
		exit(1);
	}

	if (!shaderProgram->compileShaderFromFile("triangle.fs", GL_FRAGMENT_SHADER))
	{
		printf("Fragment shader failed to compile!\n%s", shaderProgram->log().c_str());
		exit(1);
	}

	// Possibly call bindAttribLocation or bindFragDataLocation here

	if (!shaderProgram->link())
	{
		printf("Shader program failed to link!\n%s", shaderProgram->log().c_str());
		exit(1);
	}

	shaderProgram->use();

	//shaderProgram->printActiveUniforms();
	//shaderProgram->printActiveAttribs();

	GLfloat triangle_vertices[] = {
		0.0f,  0.5f, 0.0f,		
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	GLuint triangleVAO;
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	GLuint triangleVertices;
	glGenBuffers(1, &triangleVertices);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	projection_matrix = glm::perspective(45.0f, (GLfloat)DEFAULT_WINDOW_HEIGHT / (GLfloat)DEFAULT_WINDOW_WIDTH, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		triangle_model_matrix = rotate(triangle_model_matrix, (GLfloat)glfwGetTime() / 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		view_matrix = lookAt(camera_position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		shaderProgram->setUniform("model_matrix", triangle_model_matrix);
		shaderProgram->setUniform("view_matrix", view_matrix);
		shaderProgram->setUniform("projection_matrix", projection_matrix);

		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUp();

	glfwTerminate();
	return 0;
}
