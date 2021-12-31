#pragma once
#include "SDL.h"
#include "Audio/ToneSoundChannel.hpp"
#include "Audio/SweepSoundChannel.hpp"
#include "Audio/WaveSoundChannel.hpp"
#include "Audio/AudioTimer.hpp"

namespace SHG
{
	class APU
	{
	public:
		void Initialize();
		void Tick(uint32_t cycles);

		SweepSoundChannel* GetChannel1();
		ToneSoundChannel* GetChannel2();
		WaveSoundChannel* GetChannel3();

		void WriteToNR52(uint8_t value);
		uint8_t ReadNR52();

	private:
		AudioTimer frameSequencerTimer;
		uint8_t frameSequencerStep = 0;
		AudioTimer sampleCollectionTimer;

		SweepSoundChannel channel1;
		ToneSoundChannel channel2;
		WaveSoundChannel channel3;

		SDL_AudioDeviceID audioDeviceID = 0;
		std::vector<float> samples;
	};
}