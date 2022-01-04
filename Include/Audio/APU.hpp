#pragma once
#include "SDL.h"
#include "Audio/ToneSoundChannel.hpp"
#include "Audio/SweepSoundChannel.hpp"
#include "Audio/WaveSoundChannel.hpp"
#include "Audio/NoiseSoundChannel.hpp"
#include "Audio/AudioTimer.hpp"

namespace SHG
{
	class APU
	{
	public:
		bool isChannel1Enabled = true;
		bool isChannel2Enabled = true;
		bool isChannel3Enabled = true;
		bool isChannel4Enabled = true;

		void Initialize();
		void Tick(uint32_t cycles);

		SweepSoundChannel* GetChannel1();
		ToneSoundChannel* GetChannel2();
		WaveSoundChannel* GetChannel3();
		NoiseSoundChannel* GetChannel4();

		void WriteToNR52(uint8_t value);
		uint8_t ReadNR52();

	private:
		bool isSoundControllerEnabled = true;

		AudioTimer frameSequencerTimer;
		uint8_t frameSequencerStep = 0;
		AudioTimer sampleCollectionTimer;

		SweepSoundChannel channel1;
		ToneSoundChannel channel2;
		WaveSoundChannel channel3;
		NoiseSoundChannel channel4;

		SDL_AudioDeviceID audioDeviceID = 0;
		std::vector<float> samples;
	};
}