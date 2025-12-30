#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdarg>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class ShaderHelper
{
public:

	// Shader constructor
	ShaderHelper() {}

	/* Reads and parses shader code and returns a compiled
	version of the shader code. */
	GLuint readShader(const char* filename, GLenum shaderType)
	{
		std::string shaderCode;

		// Read shader code into string 
		std::ifstream file_stream(filename, std::ios::in);
		if (file_stream.is_open()) {
			shaderCode = std::string(std::istreambuf_iterator<char>(file_stream),
				std::istreambuf_iterator<char>());
			file_stream.close();
		}
		else {
			std::cout << "Could not open " << filename << std::endl;
			return 0;
		}

		// Compile shader
		GLuint shader = glCreateShader(shaderType);
		const char* shaderCStrCode = shaderCode.c_str();
		GLint shaderSourceLength = static_cast<GLuint>(strlen(shaderCStrCode));
		glShaderSource(shader, 1, &shaderCStrCode, &shaderSourceLength);
		glCompileShader(shader);

		return shader;
		
	}

	/* Compiles and links vertex and fragment shaders. Function returns
	a linked Shader Program. */
	GLuint compileShaders(const char* vShaderFilename, const char* fShaderFilename) {
		// Compile both shaders
		GLuint vShader = compileVShader(vShaderFilename);
		GLuint fShader = compileFShader(fShaderFilename);

		// Compile and link our shaders
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vShader);
		glAttachShader(shaderProgram, fShader);
		glLinkProgram(shaderProgram);

		// Check linking success
		int  success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
			return 0;
		}

		glDeleteShader(vShader);
		glDeleteShader(fShader);

		return shaderProgram;
	}

	/* Returns a full linked shader program based on list
	of ALREADY compiled shaders. */
	GLuint linkShaders(int N...) {

		va_list args;
		va_start(args, N);

		GLuint shaderProgram = glCreateProgram();

		for (int i = 0; i < N; i++) {
			GLuint shader = va_arg(args, GLuint);
			glAttachShader(shaderProgram, shader);
		}

		glLinkProgram(shaderProgram);

		// Check linking success
		int  success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
			return 0;
		}

		// Don't forget to delete your shaders.

		return shaderProgram;
	}

	// Returns a compiled vertex shader
	GLuint compileVShader(const char* vShaderFilename) {
		// Compile vertex shader
		GLuint vShader = this->readShader(vShaderFilename, GL_VERTEX_SHADER);

		// Check success of vShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(vShader);
			return 0;
		}

		return vShader;
	}

	// Returns a compiled tessellation control shader
	GLuint compileTCShader(const char* tCShaderFilename) {
		// Compile tessellation control shader
		GLuint tCShader = this->readShader(tCShaderFilename, GL_TESS_CONTROL_SHADER);

		// Check success of TCShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(tCShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(tCShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::TCS::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(tCShader);
			return 0;
		}

		return tCShader;
	}

	// Returns a compiled tessellation evaluation shader
	GLuint compileTEShader(const char* tEShaderFilename) {
		// Compile tessellation evaluation shader
		GLuint tEShader = this->readShader(tEShaderFilename, GL_TESS_EVALUATION_SHADER);

		// Check success of TEShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(tEShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(tEShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::TES::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(tEShader);
			return 0;
		}

		return tEShader;
	}

	// Returns a compiled fragment shader
	GLuint compileFShader(const char* fShaderFilename) {
		// Compile fragment shader
		GLuint fShader = this->readShader(fShaderFilename, GL_FRAGMENT_SHADER);

		// Check success of fShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(fShader);
			return 0;
		}

		return fShader;
	}
	
	// Returns a compiled compute shader
	GLuint compileCShader(const char* cShaderFilename) {
		// Compile compute shader
		GLuint cShader = this->readShader(cShaderFilename, GL_COMPUTE_SHADER);

		// Check success of cShader compilation
		int  success;
		char infoLog[512];
		glGetShaderiv(cShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(cShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
			glDeleteShader(cShader);
			return 0;
		}

		return cShader;
	}

	void setUniformMat4fv(GLuint shaderProgram, const char* uniformName, const GLfloat* uniformValue) {
		glUseProgram(shaderProgram);
		GLuint loc = glGetUniformLocation(shaderProgram, uniformName);
		glUniformMatrix4fv(loc, 1, GL_FALSE, uniformValue);
		glUseProgram(0);
	}

	void setUniform2fv(GLuint shaderProgram, const char* uniformName, const GLfloat* uniformValue) {
		glUseProgram(shaderProgram);
		GLuint loc = glGetUniformLocation(shaderProgram, uniformName);
		glUniform2fv(loc, 1, uniformValue);
		glUseProgram(0);
	}

	void setUniform3fv(GLuint shaderProgram, const char* uniformName, const GLfloat* uniformValue) {
		glUseProgram(shaderProgram);
		GLuint loc = glGetUniformLocation(shaderProgram, uniformName);
		glUniform3fv(loc, 1, uniformValue);
		glUseProgram(0);
	}

	void setUniform1f(GLuint shaderProgram, const char* uniformName, const GLfloat uniformValue) {
		glUseProgram(shaderProgram);
		GLuint loc = glGetUniformLocation(shaderProgram, uniformName);
		glUniform1f(loc, uniformValue);
		glUseProgram(0);
	}

	void useShaderProgram(GLuint shaderProgram) {
		glUseProgram(shaderProgram);
	}
};
