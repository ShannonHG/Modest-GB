#pragma once
#include <array>
#include <map>
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
		void RefreshOutputDevices();

		const std::vector<std::string>& GetAllAudioDeviceNames() const;
		void SetOutputDevice(const std::string& audioDeviceName);

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
		uint8_t ReadWavePatternRAM() const;

	private:
		bool isSoundControllerEnabled = false;

		Register8 nr50;
		Register8 nr51;

		AudioTimer frameSequencerTimer;
		uint8_t frameSequencerStep = 0;
		AudioTimer sampleCollectionTimer;

		SweepSoundChannel channel1;
		ToneSoundChannel channel2;
		WaveSoundChannel channel3;
		NoiseSoundChannel channel4;

		std::array<SoundChannel*, 4> channels = { &channel1, &channel2, &channel3, &channel4 };
		std::array<uint8_t, 4> connectionStates = { true, true, true, true };

		SDL_AudioDeviceID currentAudioDeviceID = 0;
		std::string currentAudioDeviceName;
		SDL_AudioSpec currentAudioSpec;
		std::vector<std::string> audioDeviceNames;

		std::vector<float> samples;

		void MixChannels();
		void RefreshAudioDeviceNames();
	};
}