#include "loadShader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void LoadShader::checkErrors(GLuint shaderId, GLenum parameter)
{
	GLint Result;
	int InfoLogLength;

	glGetShaderiv(shaderId, parameter, &Result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> infoLog(InfoLogLength + 1);
		glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &infoLog[0]);
		printf("%s\n", &infoLog[0]);
	}
	if (Result == GL_FALSE) {
		printf("GL error: %i\n", shaderId);
		exit(1);
	}
}

GLuint LoadShader::createGlProgram(const char* vertexFilename, const char* fragmentFilename)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	char shaderDirectory[] = "../shaders/";
	// src/shaders/
	// "../shaders/"
	std::string vertexFilepath = std::string(shaderDirectory).append(vertexFilename);
	std::string fragmentFilepath = std::string(shaderDirectory).append(fragmentFilename);

	try {
		// open files
		vShaderFile.open(vertexFilepath);
		fShaderFile.open(fragmentFilepath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	} catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	///////////////////////////////

	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	// auto vertexShaderCode = loadShader(vertexFilepath);
	// glShaderSource(vertexShaderID, 1, &vertexShaderCode, nullptr);
	glShaderSource(vertexShaderID, 1, &vShaderCode, NULL);
	glCompileShader(vertexShaderID);
	checkErrors(vertexShaderID, GL_COMPILE_STATUS);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	// auto fragmentShaderCode = loadShader(fragmentFilepath);
	// glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, nullptr);
	glShaderSource(fragmentShaderID, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShaderID);
	checkErrors(fragmentShaderID, GL_COMPILE_STATUS);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	checkErrors(programID, GL_LINK_STATUS);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}