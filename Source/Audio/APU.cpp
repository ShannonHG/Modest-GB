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
	const uint32_t FRAME_SEQUENCER_PERIOD = std::floor(GB_CLOCK_SPEED / 512.0f);

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

		sampleCollectionTimer.SetPeriod(std::floor(GB_CLOCK_SPEED / 44100.0f));
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

				if (isChannel1Enabled)
					sample += channel1.GetSample();

				if (isChannel2Enabled)
					sample += channel2.GetSample();

				if (isChannel3Enabled)
					sample += channel3.GetSample();

				if (isChannel4Enabled)
					sample += channel4.GetSample();

				samples.push_back(sample);
			}

			// TODO: Revisit
			if (samples.size() >= 4096)
			{
				if (SDL_QueueAudio(audioDeviceID, samples.data(), samples.size() * sizeof(float)) < 0)
					Logger::WriteError("SDL failed to play audio. Error: " + std::string(SDL_GetError()));

				samples.clear();
			}
		}
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

	uint8_t APU::ReadNR52()
	{
		return 
			static_cast<int8_t>(channel1.IsEnabled()) | 
			static_cast<int8_t>(channel2.IsEnabled() << 1) | 
			static_cast<int8_t>(channel3.IsEnabled() << 2) | 
			static_cast<int8_t>(channel3.IsEnabled() << 3) | 
			static_cast<int8_t>(isSoundControllerEnabled << 7);
	}

	SweepSoundChannel* APU::GetChannel1()
	{
		return &channel1;
	}

	ToneSoundChannel* APU::GetChannel2()
	{
		return &channel2;
	}

	WaveSoundChannel* APU::GetChannel3()
	{
		return &channel3;
	}

	NoiseSoundChannel* APU::GetChannel4()
	{
		return &channel4;
	}
}