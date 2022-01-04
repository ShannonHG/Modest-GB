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
		void Initialize();
		void Tick(uint32_t cycles);

		void SetChannel1ConnectionStatus(bool value);
		void SetChannel2ConnectionStatus(bool value);
		void SetChannel3ConnectionStatus(bool value);
		void SetChannel4ConnectionStatus(bool value);

		bool IsChannel1Connected() const;
		bool IsChannel2Connected() const;
		bool IsChannel3Connected() const;
		bool IsChannel4Connected() const;

		void WriteToNR10(uint8_t value);
		void WriteToNR11(uint8_t value);
		void WriteToNR12(uint8_t value);
		void WriteToNR13(uint8_t value);
		void WriteToNR14(uint8_t value);
		void WriteToNR21(uint8_t value);
		void WriteToNR22(uint8_t value);
		void WriteToNR23(uint8_t value);
		void WriteToNR24(uint8_t value);
		void WriteToNR30(uint8_t value);
		void WriteToNR31(uint8_t value);
		void WriteToNR32(uint8_t value);
		void WriteToNR33(uint8_t value);
		void WriteToNR34(uint8_t value);
		void WriteToNR41(uint8_t value);
		void WriteToNR42(uint8_t value);
		void WriteToNR43(uint8_t value);
		void WriteToNR44(uint8_t value);
		void WriteToNR50(uint8_t value);
		void WriteToNR51(uint8_t value);
		void WriteToNR52(uint8_t value);
		void WriteToWavePatternRAM(uint16_t address, uint8_t value);

		uint8_t ReadNR10() const;
		uint8_t ReadNR11() const;
		uint8_t ReadNR12() const;
		uint8_t ReadNR13() const;
		uint8_t ReadNR14() const;
		uint8_t ReadNR21() const;
		uint8_t ReadNR22() const;
		uint8_t ReadNR23() const;
		uint8_t ReadNR24() const;
		uint8_t ReadNR30() const;
		uint8_t ReadNR31() const;
		uint8_t ReadNR32() const;
		uint8_t ReadNR33() const;
		uint8_t ReadNR34() const;
		uint8_t ReadNR41() const;
		uint8_t ReadNR42() const;
		uint8_t ReadNR43() const;
		uint8_t ReadNR44() const;
		uint8_t ReadNR50() const;
		uint8_t ReadNR51() const;
		uint8_t ReadNR52() const;

	private:
		bool isChannel1Connected = true;
		bool isChannel2Connected = true;
		bool isChannel3Connected = true;
		bool isChannel4Connected = true;
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