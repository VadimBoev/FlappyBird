#ifndef AUDIO_H
#define AUDIO_H

void CreateAudioEngine();
void PlayAudio(const char* assetPath);
void PauseAudio();
void ResumeAudio();
void StopAudio();
void DestroyAudioPlayer();
void DestroyAudioEngine();

#endif // AUDIO_H