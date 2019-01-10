#pragma once

#include <glad/glad.h>

namespace LoadShader {
extern void checkErrors(GLuint shaderId, GLenum parameter);
extern const char* loadShader(const char* filepath);
extern GLuint createGlProgram(const char* vertexFilepath, const char* fragmentFilepath);

}