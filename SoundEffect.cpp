#include "SoundEffect.h"
#include <iostream>
#include <SDL.h>

SoundEffect::SoundEffect()
{
	chunk = NULL;
	channel = -1;
}

SoundEffect::~SoundEffect()
{
	Mix_FreeChunk(chunk);
}
int SoundEffect::Play()
{
	channel = Mix_PlayChannel(-1, chunk, 0);
	return channel;
}
void SoundEffect::Stop() const
{
	 Mix_HaltChannel(channel);
}
int SoundEffect::PlayLoop()
{
	channel = Mix_PlayChannel(-1, chunk, -1);
	return channel;
}
void SoundEffect::PlayAndWait()
{
	channel = Play();
	while (Mix_Playing(channel) != 0) {
		SDL_Delay(200);
	}
}
void SoundEffect::PlayAtPosition(Sint16 angle, Uint8 distance)
{
	Mix_SetPosition(Play(), angle, distance);
}
void SoundEffect::SetPosition(Sint16 angle, Uint8 distance) const
{
	Mix_SetPosition(channel, angle, distance);
}
void SoundEffect::PlayPanning(Uint8 left, Uint8 right)
{
	Mix_SetPanning(Play(), left, right);
}
void SoundEffect::SetPanning(Uint8 left, Uint8 right) const
{
	Mix_SetPanning(channel, left, right);
}
void SoundEffect::Init(const char* filename)
{
	chunk = Mix_LoadWAV(filename);
	if(chunk == NULL)
	{
		std::cerr << "*** No se encuentra: " << filename << std::endl;
	}
}

int SoundEffect::SetVolume(int volume) const
{
	if (chunk == NULL)
		return -1;
	if (volume < 0)
		volume = 0;

	if (volume > MIX_MAX_VOLUME)
		volume = MIX_MAX_VOLUME;

	return Mix_VolumeChunk(chunk, volume);
}
