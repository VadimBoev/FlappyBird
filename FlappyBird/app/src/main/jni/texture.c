#include <GLES/gl.h>
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
    glUseProgram(textureProgram);

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

    GLint positionAttrib = glGetAttribLocation(textureProgram, "aPosition");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    GLint texCoordAttrib = glGetAttribLocation(textureProgram, "aTexCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(textureProgram, "uTexture"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void RenderTexturePro(GLuint texture, float x, float y, float width, float height, float angle)
{
    glUseProgram(textureProgram);

    // calculate aspect ratio
    float aspect_ratio = (float)WindowSizeX / (float)WindowSizeY;

    // convert angle to radians
    float angle_rad = angle * M_PI / 180.0f;

    // calculate the rotation matrix
    float cos_angle = cos(angle_rad);
    float sin_angle = sin(angle_rad);

    // calculate the center of the texture in pixel coordinates
    float center_x = x + width / 2.0f;
    float center_y = y + height / 2.0f;

    // calculate the transformation matrix
    float transform[4][4] = {
        {cos_angle, -sin_angle, 0.0f, center_x * (1.0f - cos_angle) + center_y * sin_angle},
        {sin_angle, cos_angle, 0.0f, center_y * (1.0f - cos_angle) - center_x * sin_angle},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // calculate the vertices before transformation in pixel coordinates
    GLfloat vertices[] = {
        x, y, 0.0f, 0.0f,
        x + width, y, 1.0f, 0.0f,
        x + width, y + height, 1.0f, 1.0f,
        x, y + height, 0.0f, 1.0f
    };

    // apply the transformation matrix to the vertices
    for (int i = 0; i < 4; ++i) {
        float x = vertices[i * 4];
        float y = vertices[i * 4 + 1];
        vertices[i * 4] = transform[0][0] * x + transform[0][1] * y + transform[0][3];
        vertices[i * 4 + 1] = transform[1][0] * x + transform[1][1] * y + transform[1][3];
    }

    // normalize the vertices to OpenGL coordinates
    for (int i = 0; i < 4; ++i) {
        vertices[i * 4] = (2.0f * vertices[i * 4] / WindowSizeX) - 1.0f;
        vertices[i * 4 + 1] = 1.0f - (2.0f * vertices[i * 4 + 1] / WindowSizeY);
    }

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

    GLint positionAttrib = glGetAttribLocation(textureProgram, "aPosition");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    GLint texCoordAttrib = glGetAttribLocation(textureProgram, "aTexCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(textureProgram, "uTexture"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void CreateBox(uint32_t color, float posX, float posY, float width, float height)
{
    float normalizedPosX = (2.0f * posX / WindowSizeX) - 1.0f;
    float normalizedPosY = 1.0f - (2.0f * posY / WindowSizeY);
    float normalizedWidth = 2.0f * width / WindowSizeX;
    float normalizedHeight = 2.0f * height / WindowSizeY;

    float vertices[] = {
        normalizedPosX,                 normalizedPosY,
        normalizedPosX + normalizedWidth, normalizedPosY,
        normalizedPosX,                 normalizedPosY - normalizedHeight,
        normalizedPosX + normalizedWidth, normalizedPosY - normalizedHeight
    };

    glUseProgram(colorProgram);

    glVertexAttribPointer(gPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(gPositionHandle);

    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    float a = ((color >> 24) & 0xFF) / 255.0f;

    glUniform4f(gColorHandle, r, g, b, a);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}