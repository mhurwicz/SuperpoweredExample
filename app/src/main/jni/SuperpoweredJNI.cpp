#include "SuperpoweredJNI.h"
#include <SuperpoweredSimple.h>
#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

static void playerEventCallbackA(void *clientData, SuperpoweredAdvancedAudioPlayerEvent event, void * __unused value) {
    if (event == SuperpoweredAdvancedAudioPlayerEvent_LoadSuccess) {
        SuperpoweredAdvancedAudioPlayer *player = *((SuperpoweredAdvancedAudioPlayer **)clientData);
        //player->play(false);
    };
}

static bool audioProcessing(void *clientdata, short int *audioIO, int numberOfSamples, int samplerate) {
    return ((SuperpoweredJNI *)clientdata)->process(audioIO, (unsigned int)numberOfSamples, (unsigned int) samplerate);
}

SuperpoweredJNI::SuperpoweredJNI(unsigned int samplerate, unsigned int buffersize, const char *path, int fileAoffset, int fileAlength) : lastSamplerate(samplerate) {
    stereoBuffer = (float *)memalign(16, (buffersize + 16) * sizeof(float) * 2);

    player = new SuperpoweredAdvancedAudioPlayer(&player , playerEventCallbackA, samplerate, 0);

    // This one is important! If commented out, the app crashes as soon as I open & play a new file while the player is running.
    //player->open(path, fileAoffset, fileAlength);

    audioSystem = new SuperpoweredAndroidAudioIO(samplerate, buffersize, false, true, audioProcessing, this, -1, SL_ANDROID_STREAM_MEDIA, buffersize * 2);
}

SuperpoweredJNI::~SuperpoweredJNI() {
    delete audioSystem;
    delete player;
    free(stereoBuffer);
}

void SuperpoweredJNI::play() {
    player->play(false);
}

void SuperpoweredJNI::pause() {
    player->pause();
}

bool SuperpoweredJNI::process(short int *audioIO, unsigned int numberOfSamples, unsigned int samplerate) {
    if (samplerate != lastSamplerate) {
        lastSamplerate = samplerate;
        player->setSamplerate(samplerate);
    }

    bool silence = !player->process(stereoBuffer, false, numberOfSamples);
    if (!silence) SuperpoweredFloatToShortInt(stereoBuffer, audioIO, numberOfSamples);
    return !silence;
}

void SuperpoweredJNI::open(const char *path, int fileOffset, int fileLength) {
    player->open(path, fileOffset, fileLength);
}

void SuperpoweredJNI::onBackground() {
    audioSystem->onBackground();
}

void SuperpoweredJNI::onForeground() {
    audioSystem->onForeground();
}

void SuperpoweredJNI::start() {
    audioSystem->start();
}

void SuperpoweredJNI::stop() {
    audioSystem->stop();
}

static SuperpoweredJNI *example = NULL;

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_onBackground(JNIEnv *env, jobject instance) {
    example->onBackground();
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_onForeground(JNIEnv *env, jobject instance) {
    example->onForeground();
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_stop(JNIEnv *env, jobject instance) {
    example->stop();
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_start(JNIEnv *env, jobject instance) {
    example->start();
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_open(JNIEnv *env, jobject __unused instance, jstring path_,
                                                     jint fileOffset, jint fileLength) {
    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
    example->open(path, fileOffset, fileLength);
    env->ReleaseStringUTFChars(path_, path);
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_SuperpoweredExample(JNIEnv *javaEnvironment, jobject __unused obj, jint samplerate, jint buffersize, jstring apkPath, jint fileAoffset, jint fileAlength) {
    const char *path = javaEnvironment->GetStringUTFChars(apkPath, JNI_FALSE);
    example = new SuperpoweredJNI((unsigned int)samplerate, (unsigned int)buffersize, path, fileAoffset, fileAlength);
    javaEnvironment->ReleaseStringUTFChars(apkPath, path);
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_play(JNIEnv *env, jobject instance) {
    example->play();
}

extern "C" JNIEXPORT void
Java_com_superpowered_example_MainActivity_pause(JNIEnv *env, jobject instance) {
    example->pause();
}
