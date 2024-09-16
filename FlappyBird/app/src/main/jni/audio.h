#ifndef AUDIO_H
#define AUDIO_H

void createEngine();
void createAudioPlayer(const char* assetPath);
void pauseAudio();
void resumeAudio();
void stopAudio();

#endif // AUDIO_H