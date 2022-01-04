#include <vector>
#include <string>
#include "Audio/APU.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/Arithmetic.hpp"

namespace SHG
{
	const uint8_t AUDIO_CHANNELS = 1;
	const int SAMPLE_FREQUENCY = 44100;
	const uint16_t AUDIO_BUFFER_SIZE = 4096;
	const uint32_t FRAME_SEQUENCER_PERIOD = static_cast<uint32_t>(std::floor(GB_CLOCK_SPEED / 512.0f));

	void APU::Initialize()
	{
		SDL_AudioSpec spec;
		SDL_zero(spec);
		spec.freq = SAMPLE_FREQUENCY;
		spec.format = AUDIO_F32;
		spec.channels = AUDIO_CHANNELS;
		spec.samples = AUDIO_BUFFER_SIZE;

		SDL_AudioSpec outputSpec;

		// TODO: Revisit
		const char* name = SDL_GetAudioDeviceName(2, SDL_FALSE);
		Logger::WriteInfo(name);
		audioDeviceID = SDL_OpenAudioDevice(name, SDL_FALSE, &spec, &outputSpec, 0);
		SDL_PauseAudioDevice(audioDeviceID, 0);

		frameSequencerTimer.SetPeriod(FRAME_SEQUENCER_PERIOD);
		frameSequencerTimer.Restart();

		sampleCollectionTimer.SetPeriod(static_cast<uint32_t>(std::floor(GB_CLOCK_SPEED / 44100.0f)));
		sampleCollectionTimer.Restart();
	}

	void APU::Tick(uint32_t cycles)
	{
		for (uint32_t i = 0; i < cycles; i++)
		{
			if (frameSequencerTimer.Tick())
			{
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
				float sample = 0;

				if (isChannel1Connected)
					sample += channel1.GetSample();

				if (isChannel2Connected)
					sample += channel2.GetSample();

				if (isChannel3Connected)
					sample += channel3.GetSample();

				if (isChannel4Connected)
					sample += channel4.GetSample();

				samples.push_back(sample);
			}

			// TODO: Revisit
			if (samples.size() >= 4096)
			{
				if (SDL_QueueAudio(audioDeviceID, samples.data(), static_cast<uint32_t>(samples.size() * sizeof(float))) < 0)
					Logger::WriteError("SDL failed to play audio. Error: " + std::string(SDL_GetError()));

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
		// TODO: Implement
	}

	void APU::WriteToNR51(uint8_t value)
	{
		// TODO: Implement
	}

	void APU::WriteToNR52(uint8_t value)
	{
		isSoundControllerEnabled = Arithmetic::GetBit(value, 7);

		if (!isSoundControllerEnabled)
		{
			channel1.DisableSoundController();
			channel2.DisableSoundController();
			channel3.DisableSoundController();
			channel4.DisableSoundController();
		}
		else
		{
			channel1.EnableSoundController();
			channel2.EnableSoundController();
			channel3.EnableSoundController();
			channel4.EnableSoundController();
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
		// TODO: Implement
		return 0;
	}

	uint8_t APU::ReadNR51() const
	{
		// TODO: Implement
		return 0;
	}

	uint8_t APU::ReadNR52() const
	{
		return
			static_cast<int8_t>(channel1.IsEnabled()) |
			static_cast<int8_t>(channel2.IsEnabled() << 1) |
			static_cast<int8_t>(channel3.IsEnabled() << 2) |
			static_cast<int8_t>(channel3.IsEnabled() << 3) |
			static_cast<int8_t>(isSoundControllerEnabled << 7);
	}

	void APU::SetChannel1ConnectionStatus(bool value)
	{
		isChannel1Connected = value;
	}

	void APU::SetChannel2ConnectionStatus(bool value)
	{
		isChannel2Connected = value;
	}

	void APU::SetChannel3ConnectionStatus(bool value)
	{
		isChannel3Connected = value;
	}

	void APU::SetChannel4ConnectionStatus(bool value)
	{
		isChannel4Connected = value;
	}

	bool APU::IsChannel1Connected() const
	{
		return isChannel1Connected;
	}

	bool APU::IsChannel2Connected() const
	{
		return isChannel2Connected;
	}

	bool APU::IsChannel3Connected() const
	{
		return isChannel3Connected;
	}

	bool APU::IsChannel4Connected() const
	{
		return isChannel4Connected;
	}
}