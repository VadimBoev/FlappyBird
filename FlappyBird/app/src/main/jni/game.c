#include "android_native_app_glue.h"
#include <GLES2/gl2.h>
#include "game.h"
#include "utils.h"
#include "texture.h"
#include "audio.h"
#include "init.h"
#include "mouse.h"
#include <stdlib.h>
#include <math.h>

//buttons
GLuint t_pause;
GLuint t_ok;
GLuint t_menu;
GLuint t_resume;
GLuint t_score;
GLuint t_share;
GLuint t_start;

//sprites
GLuint t_0;
GLuint t_1;
GLuint t_2;
GLuint t_3;
GLuint t_4;
GLuint t_5;
GLuint t_6;
GLuint t_7;
GLuint t_8;
GLuint t_9;

GLuint t_0_small;
GLuint t_1_small;
GLuint t_2_small;
GLuint t_3_small;
GLuint t_4_small;
GLuint t_5_small;
GLuint t_6_small;
GLuint t_7_small;
GLuint t_8_small;
GLuint t_9_small;

GLuint t_background_day;
GLuint t_base;
GLuint t_bronze_medal;
GLuint t_gameover;
GLuint t_gold_medal;
GLuint t_logo;
GLuint t_message;
GLuint t_new;
GLuint t_panel;
GLuint t_pipe_green;
GLuint t_platinum_medal;
GLuint t_silver_medal;
GLuint t_sparkle_sheet;
GLuint t_yellowbird_downflap;
GLuint t_yellowbird_midflap;
GLuint t_yellowbird_upflap;

// data
uint64_t cycleTime;
bool IsDead = false;
int offsetBase = 0;
int gameSpeed = 10;
bool IsGameStarted = false;

int alpha = 0;
bool fadeOut = false;

enum GameState {
    IDLE,
    FADE_IN,
    FADE_OUT,
    READY_GAME,
    GO_GAME,
    STOP_GAME
};

enum GameState currentState = IDLE;

typedef struct {
    float x, y;
    float velocity;
    float angle;
    float width;
    float height;
    GLuint currentTexture;
    int frame;
    uint64_t lastFrameTime;
} Bird;

typedef struct {
    float x, y;
    float w, h;
    float offset;
} Pipe;

Bird bird;
Pipe pipes[2];

float Scale(float percent, bool isWidth)
{
    return (percent / 100.0f) * (isWidth ? WindowSizeX : WindowSizeY);
}

float logoY;
float birdY;
float logoVelocity;
float birdVelocity;
uint64_t timeAnimBirdForLogo;
GLuint curTextureAnimBirdForLogo;

GLuint birdTexturesForLogo[3];
int currentFrameForLogo = 0;

int Random(int min, int max) 
{
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

bool InitGame()
{
    //buttons
    t_pause = LoadTexture("buttons/pause.png");
    t_ok = LoadTexture("buttons/ok.png");
    t_menu = LoadTexture("buttons/menu.png");
    t_resume = LoadTexture("buttons/resume.png");
    t_score = LoadTexture("buttons/score.png");
    t_share = LoadTexture("buttons/share.png");
    t_start = LoadTexture("buttons/start.png");

    //sprites
    t_0 = LoadTexture("sprites/0.png");
    t_1 = LoadTexture("sprites/1.png");
    t_2 = LoadTexture("sprites/2.png");
    t_3 = LoadTexture("sprites/3.png");
    t_4 = LoadTexture("sprites/4.png");
    t_5 = LoadTexture("sprites/5.png");
    t_6 = LoadTexture("sprites/6.png");
    t_7 = LoadTexture("sprites/7.png");
    t_8 = LoadTexture("sprites/8.png");
    t_9 = LoadTexture("sprites/9.png");

    t_0_small = LoadTexture("sprites/0_small.png");
    t_1_small = LoadTexture("sprites/1_small.png");
    t_2_small = LoadTexture("sprites/2_small.png");
    t_3_small = LoadTexture("sprites/3_small.png");
    t_4_small = LoadTexture("sprites/4_small.png");
    t_5_small = LoadTexture("sprites/5_small.png");
    t_6_small = LoadTexture("sprites/6_small.png");
    t_7_small = LoadTexture("sprites/7_small.png");
    t_8_small = LoadTexture("sprites/8_small.png");
    t_9_small = LoadTexture("sprites/9_small.png");

    t_background_day = LoadTexture("sprites/background-day.png");
    t_base = LoadTexture("sprites/base.png");
    t_bronze_medal = LoadTexture("sprites/bronze-medal.png");
    t_gameover = LoadTexture("sprites/gameover.png");
    t_gold_medal = LoadTexture("sprites/gold-medal.png");
    t_logo = LoadTexture("sprites/logo.png");
    t_message = LoadTexture("sprites/message.png");
    t_new = LoadTexture("sprites/new.png");
    t_panel = LoadTexture("sprites/panel.png");
    t_pipe_green = LoadTexture("sprites/pipe-green.png");
    t_platinum_medal = LoadTexture("sprites/platinum-medal.png");
    t_silver_medal = LoadTexture("sprites/silver-medal.png");
    t_sparkle_sheet = LoadTexture("sprites/sparkle-sheet.png");
    t_yellowbird_downflap = LoadTexture("sprites/yellowbird-downflap.png");
    t_yellowbird_midflap = LoadTexture("sprites/yellowbird-midflap.png");
    t_yellowbird_upflap = LoadTexture("sprites/yellowbird-upflap.png");

    bird.x = Scale(18.52, true);
    bird.y = Scale(29.17, false);
    bird.velocity = 0.0f;
    bird.angle = 0.0f;
    bird.width = Scale(11.11, true);
    bird.height = Scale(4.17, false);
    bird.currentTexture = t_yellowbird_midflap;
    bird.frame = 0;
    bird.lastFrameTime = 0;


    pipes[0].x = Scale(100, true);
    pipes[0].y = Scale(37.5, false);
    pipes[0].w = Scale(15, true);
    pipes[0].h = Scale(37.5, false);
    pipes[0].offset = Random(Scale(-11, false), Scale(11, false));

    pipes[1].x = Scale(100, true) + Scale(60, true);
    pipes[1].y = Scale(37.5, false);
    pipes[1].w = Scale(15, true);
    pipes[1].h = Scale(37.5, false);
    pipes[1].offset = Random(Scale(-11, false), Scale(11, false));


    logoY = Scale(20.83, false);
    birdY = Scale(20.83, false);
    logoVelocity = 1.1f;
    birdVelocity = 1.1f;

    timeAnimBirdForLogo = getTickCount();
    curTextureAnimBirdForLogo = t_yellowbird_midflap;

    birdTexturesForLogo[0] = t_yellowbird_downflap;
    birdTexturesForLogo[1] = t_yellowbird_midflap;
    birdTexturesForLogo[2] = t_yellowbird_upflap;

    return true;
}

float MoveTowards(float current, float target, float maxDelta) 
{
    if (fabs(target - current) <= maxDelta) 
    {
        return target;
    }
    return current + (target > current ? maxDelta : -maxDelta);
}

void AnimateBird()
{
    uint64_t currentTime = getTickCount();
    if (currentTime - bird.lastFrameTime > 100)
    {
        bird.lastFrameTime = currentTime;
        bird.frame = (bird.frame + 1) % 3;
        switch (bird.frame) 
        {
        case 0: bird.currentTexture = t_yellowbird_downflap; break;
        case 1: bird.currentTexture = t_yellowbird_midflap; break;
        case 2: bird.currentTexture = t_yellowbird_upflap; break;
        }
    }
}

void ApplyGravity() 
{
    bird.velocity += 0.65f;
    bird.y += bird.velocity;

    float targetAngle = bird.velocity > 0 ? 90.0f : -30.0f;
    bird.angle = MoveTowards(bird.angle, targetAngle, 2.0f);

    if (bird.angle > 90.0f) bird.angle = 90.0f;
}

void Jump()
{
    bird.velocity = -13.5f;
    bird.angle = -30.0f;
}

bool CheckCollision()
{
    // detect collision with pipe's
    //for (int i = 0; i < 2; i++)
    //{
    //    if (bird.x < pipes[i].x + pipes[i].width &&
    //        bird.x + Scale(11.11, true) > pipes[i].x &&
    //        (bird.y < pipes[i].topY + pipes[i].height ||
    //            bird.y + Scale(4.17, false) > pipes[i].bottomY)) {
    //        return true;
    //    }
    //}

    // ground collision
    float baseHeight = Scale(75, false);
    if (bird.y + bird.width > baseHeight)
    {
        return true;
    }

    return false;
}

void RenderBird()
{
    RenderTexturePro(bird.currentTexture, bird.x, bird.y, bird.width, bird.height, bird.angle);
}

void RenderPipes()
{
    for (int i = 0; i < 2; i++)
    {
        RenderTexture(t_pipe_green, pipes[i].x, pipes[i].y + pipes[i].offset - (bird.height * 3.3f), pipes[i].w, -(pipes[i].h + pipes[i].offset - (bird.height * 3.3f)));
        RenderTexture(t_pipe_green, pipes[i].x, pipes[i].y + pipes[i].offset, pipes[i].w, pipes[i].h - pipes[i].offset);
    }
}

void UpdateBirdTextureForLogo()
{
    uint64_t currentTime = getTickCount();
    if (currentTime - timeAnimBirdForLogo > 100) 
    {
        timeAnimBirdForLogo = currentTime;
        currentFrameForLogo = (currentFrameForLogo + 1) % 3;
        curTextureAnimBirdForLogo = birdTexturesForLogo[currentFrameForLogo];
    }
}

void Render()
{
    //background
    RenderTexture(t_background_day, 0, 0, Scale(100, true), Scale(95.83, false));

    //cycle base texture
    if (currentState != STOP_GAME)
    {
        if (getTickCount() - cycleTime > 5)
        {
            offsetBase -= gameSpeed;
            cycleTime = getTickCount();
        }
    }

    RenderTexture(t_base, offsetBase, Scale(75, false), Scale(100, true), Scale(25, false));

    if (offsetBase < 0)
    {
        RenderTexture(t_base, Scale(100, true) + offsetBase, Scale(75, false), Scale(100, true), Scale(25, false));
    }

    if (offsetBase <= -Scale(100, true))
    {
        offsetBase = 0;
    }

    logoY += logoVelocity;
    birdY += birdVelocity;

    if (logoY > Scale(20.83, false) + 25 || logoY < Scale(20.83, false) - 25) {
        logoVelocity = -logoVelocity;
    }

    if (birdY > Scale(20.83, false) + 25 || birdY < Scale(20.83, false) - 25) {
        birdVelocity = -birdVelocity;
    }

    if (currentState == IDLE || currentState == FADE_IN)
    {
        RenderTexture(t_logo, Scale(15, true), logoY, Scale(55.56, true), Scale(5.21, false));

        UpdateBirdTextureForLogo();

        RenderTexture(curTextureAnimBirdForLogo, Scale(75, true), birdY, bird.width, bird.height);

        // button START
        if (ButtonBump(t_start, Scale(13.89, true), Scale(66.67, false), Scale(25.93, true), Scale(4.58, false)))
        {
            currentState = FADE_IN;
        }

        // button SCORE
        if (ButtonBump(t_score, Scale(60.19, true), Scale(66.67, false), Scale(25.93, true), Scale(4.58, false)))
        {

        }
    }
    else if (currentState == FADE_OUT || currentState == READY_GAME) //Ready?
    {
        RenderTexture(t_message, Scale(9.26, true), Scale(8.33, false), Scale(82.41, true), Scale(50, false));
        if (Button(0, 0, Scale(100, true), Scale(100, false)))
        {
            currentState = GO_GAME;
        }
    }
    else if (currentState == GO_GAME)
    {
        ApplyGravity();
        AnimateBird();

        for (int i = 0; i < 2; i++)
        {
            pipes[i].x -= gameSpeed;
            if (pipes[i].x < -Scale(15, true))
            {
                pipes[i].x = Scale(115, true);
                pipes[i].offset = Random(Scale(-11, false), Scale(11, false));
            }

            if (bird.x + (bird.width / 2) >= pipes[i].x + pipes[i].w &&
                bird.x + (bird.width / 2) <= pipes[i].x + pipes[i].w + gameSpeed)
            {
                PlayAudio("audio/point.mp3");
            }
        }

        if (CheckCollision())
        {
            currentState = STOP_GAME;
        }

        if (Button(0, 0, Scale(100, true), Scale(100, false))) 
        {
            Jump();
            PlayAudio("audio/wing.mp3");
        }

        RenderBird();
        RenderPipes();
    }
    else if (currentState == STOP_GAME)
    {
        RenderTexture(t_gameover, Scale(9.26, true), Scale(20.83, false), Scale(55.56, true), Scale(5.21, false));
    }

    if (currentState == FADE_IN)
    {
        alpha += 5;
        if (alpha >= 255)
        {
            alpha = 255;
            currentState = FADE_OUT;
        }
    }
    else if (currentState == FADE_OUT)
    {
        alpha -= 5;
        if (alpha <= 0)
        {
            alpha = 0;
            currentState = READY_GAME;
        }
    }

    // render black screen
    if (currentState == FADE_IN || currentState == FADE_OUT)
    {
        uint32_t color = 0x00000000 | (alpha << 24);
        CreateBox(color, 0, 0, Scale(100, true), Scale(100, false));
    }
}

bool ButtonBump(GLuint textureid, float posX, float posY, float width, float height)
{
    bool released = false;

    if (mouse.isReleased)
    {
        if (IsMouseInSquare(mouse.x, mouse.y, posX, posY, width, height))
        {
            released = true;
        }
    }

    if (released) { posY += Scale(1.04, false); }

    RenderTexture(textureid, posX, posY, width, height);

    return released;
}

bool Button(float posX, float posY, float width, float height)
{
    bool released = false;

    if (mouse.isReleased)
    {
        if (IsMouseInSquare(mouse.x, mouse.y, posX, posY, width, height))
        {
            released = true;
        }
    }

    return released;
}

void ShutdownGame()
{
    // Delete textures
    glDeleteTextures(1, &t_pause);
    glDeleteTextures(1, &t_ok);
    glDeleteTextures(1, &t_menu);
    glDeleteTextures(1, &t_resume);
    glDeleteTextures(1, &t_score);
    glDeleteTextures(1, &t_share);
    glDeleteTextures(1, &t_start);
    glDeleteTextures(1, &t_ok);

    glDeleteTextures(1, &t_0);
    glDeleteTextures(1, &t_1);
    glDeleteTextures(1, &t_2);
    glDeleteTextures(1, &t_3);
    glDeleteTextures(1, &t_4);
    glDeleteTextures(1, &t_5);
    glDeleteTextures(1, &t_6);
    glDeleteTextures(1, &t_7);
    glDeleteTextures(1, &t_8);
    glDeleteTextures(1, &t_9);

    glDeleteTextures(1, &t_0_small);
    glDeleteTextures(1, &t_1_small);
    glDeleteTextures(1, &t_2_small);
    glDeleteTextures(1, &t_3_small);
    glDeleteTextures(1, &t_4_small);
    glDeleteTextures(1, &t_5_small);
    glDeleteTextures(1, &t_6_small);
    glDeleteTextures(1, &t_7_small);
    glDeleteTextures(1, &t_8_small);
    glDeleteTextures(1, &t_9_small);

    glDeleteTextures(1, &t_background_day);
    glDeleteTextures(1, &t_base);
    glDeleteTextures(1, &t_bronze_medal);
    glDeleteTextures(1, &t_gameover);
    glDeleteTextures(1, &t_gold_medal);
    glDeleteTextures(1, &t_logo);
    glDeleteTextures(1, &t_message);
    glDeleteTextures(1, &t_new);
    glDeleteTextures(1, &t_panel);
    glDeleteTextures(1, &t_pipe_green);
    glDeleteTextures(1, &t_platinum_medal);
    glDeleteTextures(1, &t_silver_medal);
    glDeleteTextures(1, &t_sparkle_sheet);
    glDeleteTextures(1, &t_yellowbird_downflap);
    glDeleteTextures(1, &t_yellowbird_midflap);
    glDeleteTextures(1, &t_yellowbird_upflap);
}