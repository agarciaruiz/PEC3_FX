#pragma once
#include <SDL_mixer.h>

class SoundEffect {
	public:
		SoundEffect();
		~SoundEffect();
		void Init(const char* filename);
		int SetVolume(int volume) const;
		int Play();
		void Stop() const;
		int PlayLoop();
		void PlayAndWait();
		void SetPosition(Sint16 angle, Uint8 distance) const;
		void SetPanning(Uint8 left, Uint8 right) const;
		void PlayAtPosition(Sint16 angle, Uint8 distance);
		void PlayPanning(Uint8 left, Uint8 right);
	private:
		Mix_Chunk* chunk;
		int channel;
};
