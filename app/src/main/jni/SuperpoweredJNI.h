#ifndef Header_SuperpoweredExample
#define Header_SuperpoweredExample

#include <math.h>
#include <pthread.h>

#include "SuperpoweredJNI.h"
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredFilter.h>
#include <SuperpoweredRoll.h>
#include <SuperpoweredFlanger.h>
#include <SuperpoweredBandpassFilterbank.h>
#include <AndroidIO/SuperpoweredAndroidAudioIO.h>

class SuperpoweredJNI {
public:

	SuperpoweredJNI(unsigned int samplerate, unsigned int buffersize, const char *path, int fileOffset, int fileLength);
	~SuperpoweredJNI();

	// audio processing callback
	bool process(short int *output, unsigned int numberOfSamples, unsigned int samplerate);

	// player controls
	void play();
	void pause();
	void open(const char *path, int fileOffset, int fileLength);

	// lifecycle methods
	void onBackground();
	void onForeground();
	void start();
	void stop();

private:
    SuperpoweredAndroidAudioIO *audioSystem;
    SuperpoweredAdvancedAudioPlayer *player;
    float *stereoBuffer;
	unsigned int lastSamplerate;
};

#endif
