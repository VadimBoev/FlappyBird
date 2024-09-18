#ifndef TEXTURE_H
#define TEXTURE_H

#define M_PI 3.14159265358979323846f

GLuint LoadTexture(const char* assetPath);
void RenderTexture(GLuint texture, float x, float y, float width, float height);
void RenderTexturePro(GLuint texture, float x, float y, float width, float height, float angle);

void CreateBox(uint32_t color, float posX, float posY, float width, float height);

#endif // TEXTURE_H