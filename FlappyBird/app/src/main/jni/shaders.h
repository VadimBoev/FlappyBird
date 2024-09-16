#ifndef SHADERS_H
#define SHADERS_H

extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

GLuint loadShader(GLenum type, const char* source);
GLuint createProgram(const char* vertexSource, const char* fragmentSource);

#endif // SHADERS_H