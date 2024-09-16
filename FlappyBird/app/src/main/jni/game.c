#include "android_native_app_glue.h"
#include <GLES2/gl2.h>
#include "game.h"
#include "utils.h"
#include "texture.h"
#include "audio.h"

GLuint t_pause;

bool InitGame()
{
    // Load texture
    t_pause = LoadTexture("buttons/pause.png");
    if (!t_pause)
    {
        Log("Failed to load texture");
        return false;
    }

    return true;
}

uint64_t updtime = 0;
bool lol = false;
void Render()
{
	RenderTexture(t_pause, 200, 200, 100, 100);

    if (!lol)
    {
        updtime = getTickCount();
        lol = true;
    }
    
    if (getTickCount() - updtime > 1500)
    {
        PlayAudio("audio/point.mp3");
        Log("robet?");
        updtime = getTickCount();
    }

}

void ShutdownGame()
{
    // Delete textures
    glDeleteTextures(1, &t_pause);
}