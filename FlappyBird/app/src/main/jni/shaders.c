#include <GLES2/gl2.h>
#include "shaders.h"

const char* vertexShaderSource =
    "attribute vec4 aPosition;"
    "attribute vec2 aTexCoord;"
    "varying vec2 vTexCoord;"
    "void main() {"
    "    gl_Position = aPosition;"
    "    vTexCoord = aTexCoord;"
    "}";

const char* fragmentShaderSource =
    "precision mediump float;"
    "varying vec2 vTexCoord;"
    "uniform sampler2D uTexture;"
    "void main() {"
    "    gl_FragColor = texture2D(uTexture, vTexCoord);"
    "}";

GLuint loadShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}