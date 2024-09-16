#ifndef TEXTURE_H
#define TEXTURE_H

GLuint LoadTexture(const char* assetPath);
void RenderTexture(GLuint texture, float x, float y, float width, float height);

#endif // TEXTURE_H