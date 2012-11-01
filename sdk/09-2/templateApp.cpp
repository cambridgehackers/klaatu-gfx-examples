/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
};
static KlaatuAPI current_methods;

ALCdevice *al_device;
ALCcontext *al_context;
ALuint soundbuffer;
ALuint soundsource;
void templateAppInit(int width, int height)
{
    GFX_start();
    glViewport(0, 0, width, height);
    al_device = alcOpenDevice(NULL);
    al_context = alcCreateContext(al_device, NULL);
    alcMakeContextCurrent(al_context);
    alGenBuffers(1, &soundbuffer);
    MEMORY *memory = mopen((char *)"test.raw", 1);
    alBufferData(soundbuffer, AL_FORMAT_MONO16, memory->buffer, memory->size, 22050);
    memory = mclose(memory);
    alGenSources(1, &soundsource);
    alSourcei(soundsource, AL_BUFFER, soundbuffer);
    alSourcei(soundsource, AL_SOURCE_RELATIVE, AL_FALSE);
    alSourcePlay(soundsource);
}

void templateAppDraw(void)
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    int state = 0;
    alGetSourcei(soundsource, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	float playback_time = 0.0f;
	alGetSourcef(soundsource, AL_SEC_OFFSET, &playback_time);
	console_print("%f\n", playback_time);
    } else
	glClearColor(1.0f, 0.0f, 0.0f, 1.0);
    float orientation[6] = { 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
    };
    alListenerfv(AL_ORIENTATION, &orientation[0]);
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    static float y = 0.0f;
    //alSource3f( soundsource, AL_POSITION, 0.0f, y, 0.0f );
    //alSource3f( soundsource, AL_POSITION, 2.0f, y, 0.0f );
    alSource3f(soundsource, AL_POSITION, -2.0f, y, 0.0f);
    y += 0.5f;
}

void templateAppExit(void)
{
    alDeleteBuffers(1, &soundbuffer);
    alDeleteSources(1, &soundsource);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(al_context);
    alcCloseDevice(al_device);
}
int main(int argc, char *argv[])
{
    return klaatu_main(argc, argv);
}
