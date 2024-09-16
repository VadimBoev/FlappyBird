#include "audio.h"
#include "utils.h"
#include "init.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>

SLObjectItf engineObject = NULL;
SLEngineItf engineEngine;
SLObjectItf outputMixObject = NULL;
SLObjectItf playerObject = NULL;
SLPlayItf playerPlay;
SLSeekItf playerSeek;

void createEngine()
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to create engine");
        return;
    }

    // init engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to realize engine");
        return;
    }

    // get interface engine
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to get engine interface");
        return;
    }

    // create output sound
    const SLInterfaceID ids[1] = { SL_IID_ENVIRONMENTALREVERB };
    const SLboolean req[1] = { SL_BOOLEAN_FALSE };
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to create output mix");
        return;
    }

    // init output
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to realize output mix");
        return;
    }
}

void createAudioPlayer(const char* assetPath)
{
    SLresult result;

    AAsset* audioAsset = AAssetManager_open(g_App->activity->assetManager, assetPath, AASSET_MODE_BUFFER);
    if (!audioAsset)
    {
        Log("Failed to open asset file");
        return;
    }

    off_t start, length;
    int fd = AAsset_openFileDescriptor(audioAsset, &start, &length);
    AAsset_close(audioAsset);

    if (fd < 0)
    {
        Log("Failed to get file descriptor");
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
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc, &audioSnk, 3, ids, req);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to create audio player");
        return;
    }

    // init player
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to realize player");
        return;
    }

    // get interface player
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to get play interface");
        return;
    }

    // get interface seek
    result = (*playerObject)->GetInterface(playerObject, SL_IID_SEEK, &playerSeek);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to get seek interface");
        return;
    }

    // go play
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS)
    {
        Log("Failed to start playing");
        return;
    }
}

void pauseAudio()
{
    SLresult result;
    if (playerPlay)
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PAUSED);
        if (result != SL_RESULT_SUCCESS)
        {
            Log("Failed to pause playing");
        }
    }
}

void resumeAudio()
{
    SLresult result;
    if (playerPlay)
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS)
        {
            Log("Failed to resume playing");
        }
    }
}

void stopAudio()
{
    SLresult result;
    if (playerPlay)
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_STOPPED);
        if (result != SL_RESULT_SUCCESS)
        {
            Log("Failed to stop playing");
        }
    }
}