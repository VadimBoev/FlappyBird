#include <GLES2/gl2.h>
#include <android/asset_manager.h>
#include "upng.h"
#include "texture.h"
#include "utils.h"
#include "init.h"

GLuint LoadTexture(const char* assetPath)
{
    AAsset* file = AAssetManager_open(g_App->activity->assetManager, assetPath, AASSET_MODE_BUFFER);
    if (!file)
    {
        Log("Failed to open asset file: %s", assetPath);
        return 0;
    }

    unsigned char* buffer = (unsigned char*)AAsset_getBuffer(file);
    unsigned long len_file = AAsset_getLength(file);

    upng_t* png = upng_new_from_bytes(buffer, len_file);
    if (png == NULL)
    {
        Log("Error creating PNG from file: %s", assetPath);
        AAsset_close(file);
        return 0;
    }

    upng_decode(png);
    if (upng_get_error(png) != UPNG_EOK)
    {
        Log("Error decoding PNG from file: %s", assetPath);
        upng_free(png);
        AAsset_close(file);
        return 0;
    }

    unsigned width = upng_get_width(png);
    unsigned height = upng_get_height(png);

    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, upng_get_buffer(png));

    upng_free(png);

    AAsset_close(file);

    if (!texture)
    {
        Log("Error load texture '%s'", assetPath);
        return 0;
    }
    else
    {
        Log("Texture '%s' is loaded!", assetPath);
    }

    return texture;
}

void RenderTexture(GLuint texture, float x, float y, float width, float height)
{
    glUseProgram(program);

    // pixel coordinates to normal coordinates
    float normalized_x = (2.0f * x / WindowSizeX) - 1.0f;
    float normalized_y = 1.0f - (2.0f * y / WindowSizeY);
    float normalized_width = 2.0f * width / WindowSizeX;
    float normalized_height = 2.0f * height / WindowSizeY;

    GLfloat vertices[] = {
        normalized_x, normalized_y, 0.0f, 0.0f,
        normalized_x + normalized_width, normalized_y, 1.0f, 0.0f,
        normalized_x + normalized_width, normalized_y - normalized_height, 1.0f, 1.0f,
        normalized_x, normalized_y - normalized_height, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint positionAttrib = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    GLint texCoordAttrib = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}