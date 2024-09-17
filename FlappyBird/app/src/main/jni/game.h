#ifndef GAME_H
#define GAME_H

bool InitGame();
void Render();
void ShutdownGame();
bool Button(GLuint textureid, float posX, float posY, float width, float height);


#endif // GAME_H