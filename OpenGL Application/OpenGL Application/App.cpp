#include "glew.h"
#include "glfw3.h"
#include "glm.hpp"

#include "matrix_transform.hpp"
#include "type_ptr.hpp"

#include <vector>
#include <string>
#include <fstream>

glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 5.0f);

glm::mat4 triangle_model_matrix;
glm::mat4 view_matrix;
glm::mat4 projection_matrix;

const GLuint DEFAULT_WINDOW_WIDTH = 800, DEFAULT_WINDOW_HEIGHT = 800;
const GLfloat CAMERA_MOVEMENT_SPEED = 0.02f;

double ypos_old = -1;

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, VertexShaderID);
	glAttachShader(program_id, FragmentShaderID);

	glBindAttribLocation(program_id, 0, "in_Position");
	glBindFragDataLocation(program_id, 0, "out_Color");

	glLinkProgram(program_id);

	// Check the program
	glGetProgramiv(program_id, GL_LINK_STATUS, &Result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(program_id, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(program_id, VertexShaderID);
	glDetachShader(program_id, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return program_id;
}

void framebuffer_size_callback(GLFWwindow* window, int new_screen_width, int new_screen_height)
{
	glViewport(0, 0, new_screen_width, new_screen_height);
	projection_matrix = glm::perspective(45.0f, (GLfloat)new_screen_width / (GLfloat)new_screen_height, 0.1f, 10.0f);
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

	GLuint shader_program = loadShaders("triangle.vs", "triangle.fs");
	glUseProgram(shader_program);

	GLint modelMatrixLoc = glGetUniformLocation(shader_program, "model_matrix");
	GLint viewMatrixLoc = glGetUniformLocation(shader_program, "view_matrix");
	GLint projectionMatrixLoc = glGetUniformLocation(shader_program, "projection_matrix");

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
		
		glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, value_ptr(triangle_model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, value_ptr(view_matrix));
		glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, value_ptr(projection_matrix));

		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
