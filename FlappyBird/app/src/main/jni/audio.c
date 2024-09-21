#include "audio.h"
#include "utils.h"
#include "init.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include <stdlib.h>
#include <unistd.h>  // close

#define MAX_PLAYERS 5
#define MAX_USES 5

typedef struct {
    SLObjectItf playerObject;
    SLPlayItf playerPlay;
    SLSeekItf playerSeek;
    int fd;  // file descriptor for the audio file
    int uses;
} AudioPlayer;

SLObjectItf engineObject = NULL;
SLEngineItf engineEngine;
SLObjectItf outputMixObject = NULL;
AudioPlayer players[MAX_PLAYERS] = { { NULL, NULL, NULL, -1, 0 } };
int currentPlayerIndex = 0;

void DestroyUnusedPlayers()
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i].playerPlay)
        {
            SLuint32 state;
            SLresult result = (*players[i].playerPlay)->GetPlayState(players[i].playerPlay, &state);
            if (result == SL_RESULT_SUCCESS && state == SL_PLAYSTATE_STOPPED)
            {
                Log("Destroying unused player %d", i);
                (*players[i].playerObject)->Destroy(players[i].playerObject);
                close(players[i].fd);  // close the file descriptor
                players[i].playerObject = NULL;
                players[i].playerPlay = NULL;
                players[i].playerSeek = NULL;
                players[i].fd = -1;
                players[i].uses = 0;
            }
        }
    }
}

void CreateAudioEngine()
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to create engine: %d", result);
        return;
    }

    // init engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to realize engine: %d", result);
        return;
    }

    // get interface engine
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to get engine interface: %d", result);
        return;
    }

    // create output sound
    const SLInterfaceID ids[1] = { SL_IID_ENVIRONMENTALREVERB };
    const SLboolean req[1] = { SL_BOOLEAN_FALSE };
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to create output mix: %d", result);
        return;
    }

    // init output
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to realize output mix: %d", result);
        return;
    }
}

void CreateAudioPlayer(AudioPlayer* player, const char* assetPath)
{
    SLresult result;

    AAsset* audioAsset = AAssetManager_open(g_App->activity->assetManager, assetPath, AASSET_MODE_BUFFER);
    if (!audioAsset)
    {
        Log("Failed to open asset file: %s", assetPath);
        return;
    }

    off_t start, length;
    int fd = AAsset_openFileDescriptor(audioAsset, &start, &length);
    AAsset_close(audioAsset);

    if (fd < 0)
    {
        Log("Failed to get file descriptor for asset: %s", assetPath);
        return;
    }

    // create source data
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_MP3 };
    SLDataSource audioSrc = { &loc_fd, &format_mime };

    // setting receive data
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
    SLDataSink audioSnk = { &loc_outmix, NULL };

    // create player
    const SLInterfaceID ids[3] = { SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME };
    const SLboolean req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &player->playerObject, &audioSrc, &audioSnk, 3, ids, req);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to create audio player: %d", result);
        close(fd);
        return;
    }

    // init player
    result = (*player->playerObject)->Realize(player->playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to realize player: %d", result);
        close(fd);
        return;
    }

    // get interface player
    result = (*player->playerObject)->GetInterface(player->playerObject, SL_IID_PLAY, &player->playerPlay);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to get play interface: %d", result);
        close(fd);
        return;
    }

    // get interface seek
    result = (*player->playerObject)->GetInterface(player->playerObject, SL_IID_SEEK, &player->playerSeek);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to get seek interface: %d", result);
        close(fd);
        return;
    }

    player->fd = fd;  // save descriptor
    player->uses = 0; // reset counter
    Log("Successfully created audio player for asset: %s", assetPath);
}

void PlayAudio(const char* assetPath)
{
    SLresult result;

    DestroyUnusedPlayers();

    // find an available player
    SLPlayItf playerPlay = NULL;
    int availableIndex = -1;
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i].playerPlay)
        {
            SLuint32 state;
            result = (*players[i].playerPlay)->GetPlayState(players[i].playerPlay, &state);
            if (result == SL_RESULT_SUCCESS && state == SL_PLAYSTATE_STOPPED)
            {
                playerPlay = players[i].playerPlay;
                availableIndex = i;
                Log("Reusing player %d for asset: %s", i, assetPath);
                break;
            }
        }
    }

    // if no available player, create a new one
    if (!playerPlay)
    {
        for (int i = 0; i < MAX_PLAYERS; ++i)
        {
            if (!players[i].playerPlay)
            {
                CreateAudioPlayer(&players[i], assetPath);
                playerPlay = players[i].playerPlay;
                availableIndex = i;
                Log("Created new player %d for asset: %s", i, assetPath);
                break;
            }
        }
    }

    // if still no available player, destroy the next player in the cycle and create a new one
    if (!playerPlay)
    {
        int oldestIndex = currentPlayerIndex;
        currentPlayerIndex = (currentPlayerIndex + 1) % MAX_PLAYERS; // move to the next player in the cycle

        Log("Destroying player %d to create a new one for asset: %s", oldestIndex, assetPath);
        (*players[oldestIndex].playerObject)->Destroy(players[oldestIndex].playerObject);
        close(players[oldestIndex].fd);  // close the file descriptor
        players[oldestIndex].playerObject = NULL;
        players[oldestIndex].playerPlay = NULL;
        players[oldestIndex].playerSeek = NULL;
        players[oldestIndex].fd = -1;
        players[oldestIndex].uses = 0;

        CreateAudioPlayer(&players[oldestIndex], assetPath);
        playerPlay = players[oldestIndex].playerPlay;
        availableIndex = oldestIndex;
        Log("Created new player %d for asset: %s", oldestIndex, assetPath);
    }

    // start playing
    if (playerPlay)
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS)
        {
            Log("Failed to start playing: %d", result);
            return;
        }
        Log("Started playing asset: %s", assetPath);
        players[availableIndex].uses++;

        // check if the player has reached the maximum uses
        if (players[availableIndex].uses >= MAX_USES)
        {
            Log("Player %d has reached the maximum uses, destroying and creating a new one", availableIndex);
            (*players[availableIndex].playerObject)->Destroy(players[availableIndex].playerObject);
            close(players[availableIndex].fd);  // close the file descriptor
            players[availableIndex].playerObject = NULL;
            players[availableIndex].playerPlay = NULL;
            players[availableIndex].playerSeek = NULL;
            players[availableIndex].fd = -1;
            players[availableIndex].uses = 0;

            CreateAudioPlayer(&players[availableIndex], assetPath);
            playerPlay = players[availableIndex].playerPlay;
            Log("Created new player %d for asset: %s", availableIndex, assetPath);
        }
    }
    else
    {
        Log("No available player to play asset: %s", assetPath);
    }
}

void PauseAudio()
{
    SLresult result;
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i].playerPlay)
        {
            result = (*players[i].playerPlay)->SetPlayState(players[i].playerPlay, SL_PLAYSTATE_PAUSED);
            if (result != SL_RESULT_SUCCESS)
            {
                Log("Failed to pause playing for player %d: %d", i, result);
            }
            else
            {
                Log("Paused playing for player %d", i);
            }
        }
    }
}

void ResumeAudio()
{
    SLresult result;
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i].playerPlay)
        {
            result = (*players[i].playerPlay)->SetPlayState(players[i].playerPlay, SL_PLAYSTATE_PLAYING);
            if (result != SL_RESULT_SUCCESS)
            {
                Log("Failed to resume playing for player %d: %d", i, result);
            }
            else
            {
                Log("Resumed playing for player %d", i);
            }
        }
    }
}

void StopAudio()
{
    SLresult result;
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i].playerPlay)
        {
            result = (*players[i].playerPlay)->SetPlayState(players[i].playerPlay, SL_PLAYSTATE_STOPPED);
            if (result != SL_RESULT_SUCCESS)
            {
                Log("Failed to stop playing for player %d: %d", i, result);
            }
            else
            {
                Log("Stopped playing for player %d", i);
            }
        }
    }
}

void DestroyAudioPlayer()
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i].playerObject != NULL)
        {
            (*players[i].playerObject)->Destroy(players[i].playerObject);
            close(players[i].fd);  // close the file descriptor
            players[i].playerObject = NULL;
            players[i].playerPlay = NULL;
            players[i].playerSeek = NULL;
            players[i].fd = -1;
            players[i].uses = 0;
            Log("Destroyed player %d", i);
        }
    }
}

void DestroyAudioEngine()
{
    if (outputMixObject != NULL)
    {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        Log("Destroyed output mix object");
    }

    if (engineObject != NULL)
    {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
        Log("Destroyed engine object");
    }
}