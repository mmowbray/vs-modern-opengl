#include "GLSLProgram.h"
#include <vector>
#include <fstream>
#include <type_ptr.hpp>

GLSLProgram::GLSLProgram()
{
	handle = glCreateProgram();
	linked = false;
}

bool GLSLProgram::compileShaderFromString(const string & source, GLuint type)
{
	GLuint shaderID = glCreateShader(type);

	// Compile Shader
	char const * SourcePointer = source.c_str();
	glShaderSource(shaderID, 1, &SourcePointer, nullptr);
	glCompileShader(shaderID);

	// Check if the compiling worked
	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_TRUE)
	{
		//linked = true;
		glAttachShader(handle, shaderID);
		return true;
	}
	else
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength > 0) {
			std::vector<char> infoLog(maxLength + 1);
			glGetShaderInfoLog(shaderID, maxLength, nullptr, infoLog.data());
			logString = string(std::begin(infoLog), std::end(infoLog));
		}

		return false;
	}
}

bool GLSLProgram::compileShaderFromFile(const char * fileName, GLuint type)
{
	if(fileExists(fileName))
	{
		string shaderCode;
		std::ifstream ShaderStream(fileName, std::ios::in);
		if (ShaderStream.is_open()) {
			std::string Line = "";
			while (getline(ShaderStream, Line))
				shaderCode += "\n" + Line;
			ShaderStream.close();

			return compileShaderFromString(shaderCode, type);
		}

		return true;
	}
	return false;
}

bool GLSLProgram::link()
{
	glLinkProgram(handle);

	// Check if the linking worked
	GLint isLinked = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &isLinked);
	if(isLinked == GL_TRUE)
	{
		linked = true;
	}else
	{
		GLint maxLength = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(handle, maxLength, NULL, infoLog.data());
		logString = string(begin(infoLog), end(infoLog));

		//The program is useless now.
		glDeleteProgram(handle);
	}

	return linked;
}

void GLSLProgram::use()
{
	if (linked)
		glUseProgram(handle);
}

string GLSLProgram::log()
{
	return logString;
}

int GLSLProgram::getHandle()
{
	return handle;
}

bool GLSLProgram::isLinked()
{
	return linked;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char * name)
{
	glBindAttribLocation(handle, location, name);
}

void GLSLProgram::bindFragDataLocation(GLuint location, const char * name)
{
	glBindFragDataLocation(handle, location, name);
}

void GLSLProgram::setUniform(const char *name, float x, float y, float z)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniform3f(location, x, y, z);
}

void GLSLProgram::setUniform(const char *name, const glm::vec3 & v)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniform3fv(location, 3, glm::value_ptr(v));
}

void GLSLProgram::setUniform(const char *name, const glm::vec4 & v)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniform4fv(location, 4, glm::value_ptr(v));
}

void GLSLProgram::setUniform(const char *name, const glm::mat4 & m)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m));
}

void GLSLProgram::setUniform(const char *name, const glm::mat3 & m)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(m));
}

void GLSLProgram::setUniform(const char *name, float val)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniform1f(location, val);
}

void GLSLProgram::setUniform(const char *name, int val)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniform1i(location, val);
}

void GLSLProgram::setUniform(const char *name, bool val)
{
	GLint location = glGetUniformLocation(handle, name);

	if (location != -1)
		glUniform1i(location, val);
}

void GLSLProgram::printActiveUniforms()
{
	//@TODO
}

bool GLSLProgram::fileExists(const string & fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

int GLSLProgram::getUniformLocation(const char * name)
{
	return glGetUniformLocation(handle, name);
}
