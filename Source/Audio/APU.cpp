#include <vector>
#include <string>
#include "Audio/APU.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/Arithmetic.hpp"

namespace SHG
{
	const std::string APU_MESSAGE_HEADER = "[APU]";
	const uint8_t AUDIO_CHANNELS = 2;
	const int SAMPLE_FREQUENCY = 44100;
	const uint16_t AUDIO_BUFFER_SIZE = 4096;
	const uint32_t FRAME_SEQUENCER_PERIOD = static_cast<uint32_t>(std::floor(GB_CLOCK_SPEED / 512.0f));
	const uint32_t SAMPLE_COLLECTION_TIMER_PERIOD = static_cast<uint32_t>(std::floor(GB_CLOCK_SPEED / static_cast<float>(SAMPLE_FREQUENCY)));
	const float MASTER_VOLUME_MULTIPLIER = 0.075f;

	void APU::Initialize()
	{
		SDL_zero(currentAudioSpec);
		currentAudioSpec.freq = SAMPLE_FREQUENCY;
		currentAudioSpec.format = AUDIO_F32;
		currentAudioSpec.channels = AUDIO_CHANNELS;
		currentAudioSpec.samples = AUDIO_BUFFER_SIZE;

		// By default, set the output device to the first available audio device.
		SetOutputDevice(SDL_GetAudioDeviceName(0, SDL_FALSE));
		RefreshAudioDeviceNames();

		frameSequencerTimer.Restart(FRAME_SEQUENCER_PERIOD);
		sampleCollectionTimer.Restart(SAMPLE_COLLECTION_TIMER_PERIOD);
	}

	void APU::Reset()
	{
		channel1.Reset();
		channel2.Reset();
		channel3.Reset();
		channel4.Reset();

		frameSequencerTimer.Restart(FRAME_SEQUENCER_PERIOD);
		sampleCollectionTimer.Restart(SAMPLE_COLLECTION_TIMER_PERIOD);

		samples.clear();

		if (currentAudioDeviceID > 0)
			SDL_ClearQueuedAudio(currentAudioDeviceID);
	}

	float APU::GetMasterVolume() const
	{
		return masterVolume;
	}

	void APU::SetMasterVolume(float volume)
	{
		masterVolume = volume;
	}

	void APU::Mute(bool value)
	{
		isMuted = value;

		if (isMuted)
			samples.clear();
	}

	bool APU::IsMuted() const
	{
		return isMuted;
	}

	const std::string& APU::GetCurrentOutputDeviceName() const
	{
		return currentAudioDeviceName;
	}

	const std::vector<std::string>& APU::GetAllOutputDeviceNames() const
	{
		return audioDeviceNames;
	}

	void APU::SetOutputDevice(const std::string& audioDeviceName)
	{
		SDL_AudioDeviceID result = SDL_OpenAudioDevice(audioDeviceName.c_str(), SDL_FALSE, &currentAudioSpec, nullptr, 0);

		if (result == 0)
		{
			Logger::WriteWarning("Failed to select output device: " + audioDeviceName, APU_MESSAGE_HEADER);
			return;
		}

		// Close the existing audio device, if any.
		if (currentAudioDeviceID > 0)
			SDL_CloseAudioDevice(currentAudioDeviceID);

		currentAudioDeviceID = result;
		currentAudioDeviceName = audioDeviceName;

		SDL_PauseAudioDevice(currentAudioDeviceID, SDL_FALSE);
		Logger::WriteInfo("Selected output device: " + currentAudioDeviceName, APU_MESSAGE_HEADER);
	}

	void APU::RefreshOutputDevices()
	{
		// Check if any audio devices were added or removed.
		if (SDL_GetNumAudioDevices(SDL_FALSE) != audioDeviceNames.size())
		{
			Logger::WriteInfo("Output devices changed.", APU_MESSAGE_HEADER);
			RefreshAudioDeviceNames();
		}
	}

	void APU::Tick(uint32_t cycles)
	{
		for (uint32_t i = 0; i < cycles; i++)
		{
			if (frameSequencerTimer.Tick())
			{
				frameSequencerTimer.Restart(FRAME_SEQUENCER_PERIOD);

				switch (frameSequencerStep)
				{
				case 0:
					channel1.TickLengthControlTimer();
					channel2.TickLengthControlTimer();
					channel3.TickLengthControlTimer();
					channel4.TickLengthControlTimer();
					break;
				case 1:
					break;
				case 2:
					channel1.TickSweepTimer();
					channel1.TickLengthControlTimer();
					channel2.TickLengthControlTimer();
					channel3.TickLengthControlTimer();
					channel4.TickLengthControlTimer();
					break;
				case 3:
					break;
				case 4:
					channel1.TickLengthControlTimer();
					channel2.TickLengthControlTimer();
					channel3.TickLengthControlTimer();
					channel4.TickLengthControlTimer();
					break;
				case 5:
					break;
				case 6:
					channel1.TickSweepTimer();
					channel1.TickLengthControlTimer();
					channel2.TickLengthControlTimer();
					channel3.TickLengthControlTimer();
					channel4.TickLengthControlTimer();
					break;
				case 7:
					channel1.TickVolumeEnvelopeTimer();
					channel2.TickVolumeEnvelopeTimer();
					channel4.TickVolumeEnvelopeTimer();
					break;
				}

				frameSequencerStep = (frameSequencerStep + 1) % 8;
			}

			channel1.TickFrequencyTimer();
			channel2.TickFrequencyTimer();
			channel3.TickFrequencyTimer();
			channel4.TickFrequencyTimer();

			if (sampleCollectionTimer.Tick())
			{
				sampleCollectionTimer.Restart(SAMPLE_COLLECTION_TIMER_PERIOD);

				// Mix the sound channels, and add the samples to the buffer.
				MixChannels();
			}

			if (samples.size() >= AUDIO_BUFFER_SIZE)
			{
				if (SDL_QueueAudio(currentAudioDeviceID, samples.data(), static_cast<uint32_t>(samples.size() * sizeof(float))) < 0)
					Logger::WriteError("SDL failed to play audio. Error: " + std::string(SDL_GetError()), APU_MESSAGE_HEADER);

				samples.clear();
			}
		}
	}

	void APU::WriteToNR10(uint8_t value)
	{
		channel1.WriteToNRX0(value);
	}

	void APU::WriteToNR11(uint8_t value)
	{
		channel1.WriteToNRX1(value);
	}

	void APU::WriteToNR12(uint8_t value)
	{
		channel1.WriteToNRX2(value);
	}

	void APU::WriteToNR13(uint8_t value)
	{
		channel1.WriteToNRX3(value);
	}

	void APU::WriteToNR14(uint8_t value)
	{
		channel1.WriteToNRX4(value);
	}

	void APU::WriteToNR21(uint8_t value)
	{
		channel2.WriteToNRX1(value);
	}

	void APU::WriteToNR22(uint8_t value)
	{
		channel2.WriteToNRX2(value);
	}

	void APU::WriteToNR23(uint8_t value)
	{
		channel2.WriteToNRX3(value);
	}

	void APU::WriteToNR24(uint8_t value)
	{
		channel2.WriteToNRX4(value);
	}

	void APU::WriteToNR30(uint8_t value)
	{
		channel3.WriteToNRX0(value);
	}

	void APU::WriteToNR31(uint8_t value)
	{
		channel3.WriteToNRX1(value);
	}

	void APU::WriteToNR32(uint8_t value)
	{
		channel3.WriteToNRX2(value);
	}

	void APU::WriteToNR33(uint8_t value)
	{
		channel3.WriteToNRX3(value);
	}

	void APU::WriteToNR34(uint8_t value)
	{
		channel3.WriteToNRX4(value);
	}

	void APU::WriteToNR41(uint8_t value)
	{
		channel4.WriteToNRX1(value);
	}

	void APU::WriteToNR42(uint8_t value)
	{
		channel4.WriteToNRX2(value);
	}

	void APU::WriteToNR43(uint8_t value)
	{
		channel4.WriteToNRX3(value);
	}

	void APU::WriteToNR44(uint8_t value)
	{
		channel4.WriteToNRX4(value);
	}

	void APU::WriteToNR50(uint8_t value)
	{
		nr50.Write(value);
	}

	void APU::WriteToNR51(uint8_t value)
	{
		nr51.Write(value);
	}

	void APU::WriteToNR52(uint8_t value)
	{
		isSoundControllerEnabled = Arithmetic::GetBit(value, 7);

		if (isSoundControllerEnabled)
			frameSequencerStep = 0;

		for (uint8_t i = 0; i < 4; i++)
		{
			if (!isSoundControllerEnabled)
				channels[i]->DisableSoundController();
			else
				channels[i]->EnableSoundController();
		}
	}

	void APU::WriteToWavePatternRAM(uint16_t address, uint8_t value)
	{
		channel3.WriteToWavePatternRAM(address, value);
	}

	uint8_t APU::ReadNR10() const
	{
		return channel1.ReadNRX0();
	}

	uint8_t APU::ReadNR11() const
	{
		return channel1.ReadNRX1();
	}

	uint8_t APU::ReadNR12() const
	{
		return channel1.ReadNRX2();
	}

	uint8_t APU::ReadNR13() const
	{
		return channel1.ReadNRX3();
	}

	uint8_t APU::ReadNR14() const
	{
		return channel1.ReadNRX4();
	}

	uint8_t APU::ReadNR21() const
	{
		return channel2.ReadNRX1();
	}

	uint8_t APU::ReadNR22() const
	{
		return channel2.ReadNRX2();
	}

	uint8_t APU::ReadNR23() const
	{
		return channel2.ReadNRX3();
	}

	uint8_t APU::ReadNR24() const
	{
		return channel2.ReadNRX4();
	}

	uint8_t APU::ReadNR30() const
	{
		return channel3.ReadNRX0();
	}

	uint8_t APU::ReadNR31() const
	{
		return channel3.ReadNRX1();
	}

	uint8_t APU::ReadNR32() const
	{
		return channel3.ReadNRX2();
	}

	uint8_t APU::ReadNR33() const
	{
		return channel3.ReadNRX3();
	}

	uint8_t APU::ReadNR34() const
	{
		return channel3.ReadNRX4();
	}

	uint8_t APU::ReadNR41() const
	{
		return channel4.ReadNRX1();
	}

	uint8_t APU::ReadNR42() const
	{
		return channel4.ReadNRX2();
	}

	uint8_t APU::ReadNR43() const
	{
		return channel4.ReadNRX3();
	}

	uint8_t APU::ReadNR44() const
	{
		return channel4.ReadNRX4();
	}

	uint8_t APU::ReadNR50() const
	{
		return nr50.Read();
	}

	uint8_t APU::ReadNR51() const
	{
		return nr51.Read();
	}

	uint8_t APU::ReadNR52() const
	{
		return
			(static_cast<int8_t>(channel1.IsEnabled()) |
				static_cast<int8_t>(channel2.IsEnabled() << 1) |
				static_cast<int8_t>(channel3.IsEnabled() << 2) |
				static_cast<int8_t>(channel4.IsEnabled() << 3) |
				static_cast<int8_t>(isSoundControllerEnabled << 7)) | 0x70;
	}

	uint8_t APU::ReadWavePatternRAM() const
	{
		return channel3.ReadWavePatternRAM();
	}

	void APU::SetChannel1ConnectionStatus(bool value)
	{
		connectionStates[0] = value;
	}

	void APU::SetChannel2ConnectionStatus(bool value)
	{
		connectionStates[1] = value;
	}

	void APU::SetChannel3ConnectionStatus(bool value)
	{
		connectionStates[2] = value;
	}

	void APU::SetChannel4ConnectionStatus(bool value)
	{
		connectionStates[3] = value;
	}

	bool APU::IsChannel1Connected() const
	{
		return connectionStates[0];
	}

	bool APU::IsChannel2Connected() const
	{
		return connectionStates[1];
	}

	bool APU::IsChannel3Connected() const
	{
		return connectionStates[2];
	}

	bool APU::IsChannel4Connected() const
	{
		return connectionStates[3];
	}

	void APU::MixChannels()
	{
		if (isMuted)
			return;

		float right = 0;
		float left = 0;

		uint8_t rightVolume = nr50.Read(0, 2) + 1;
		uint8_t leftVolume = nr50.Read(4, 6) + 1;

		for (uint8_t i = 0; i < 4; i++)
		{
			if (!connectionStates[i])
				continue;

			// Check if NR51 states that the sound should be sent to the right audio channel.
			if (nr51.Read(i))
				right += channels[i]->GetSample() * rightVolume;

			// Check if NR51 states that the sound should be sent to the left audio channel.
			if (nr51.Read(i + 4))
				left += channels[i]->GetSample() * leftVolume;
		}

		// Average the samples.
		right /= 4.0f;
		left /= 4.0f;

		// In stereo mode, SDL expects the samples in left/right ordering, so the left sample should be pushed first
		samples.push_back(left * (masterVolume * MASTER_VOLUME_MULTIPLIER));
		samples.push_back(right * (masterVolume * MASTER_VOLUME_MULTIPLIER));
	}

	void APU::RefreshAudioDeviceNames()
	{
		audioDeviceNames.clear();

		bool isCurrentDeviceFound = false;
		int outputDeviceCount = SDL_GetNumAudioDevices(SDL_FALSE);

		for (int i = 0; i < outputDeviceCount; i++)
		{
			const char* name = SDL_GetAudioDeviceName(i, SDL_FALSE);

			// Is the currently selected audio device still connected?
			if (name == currentAudioDeviceName)
				isCurrentDeviceFound = true;

			audioDeviceNames.push_back(name);
		}

		// Is the currently selected audio device is no longer connected, then choose a new device.
		if (!isCurrentDeviceFound)
			SetOutputDevice(SDL_GetAudioDeviceName(std::max(outputDeviceCount - 1, 0), SDL_FALSE));
	}
}