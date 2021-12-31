#include "Audio/WaveSoundChannel.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	const uint16_t WAVE_PATTERN_RAM_START_ADDRESS = 0xFF30;
	const uint16_t WAVE_PATTERN_RAM_END_ADDRESS = 0xFF3F;
	const uint8_t NUMBER_OF_SAMPLES = 32;

	WaveSoundChannel::WaveSoundChannel()
	{
		samples = std::vector<uint8_t>(NUMBER_OF_SAMPLES);
	}

	void WaveSoundChannel::WriteToWavePatternRAM(uint16_t address, uint8_t value)
	{
		if (address < WAVE_PATTERN_RAM_START_ADDRESS || address > WAVE_PATTERN_RAM_END_ADDRESS)
		{
			Logger::WriteError("Wave pattern RAM address is out of bounds: " + GetHexString16(address));
			return;
		}

		samples[address - WAVE_PATTERN_RAM_START_ADDRESS] = (value >> 4) & 0b1111;
		samples[(address - WAVE_PATTERN_RAM_START_ADDRESS) + 1] = value & 0b1111;
	}

	uint32_t WaveSoundChannel::GetLengthTimerPeriod() const
	{
		return nrx1;
	}

	float WaveSoundChannel::GenerateSample() const
	{
		return samples[sampleIndex] >> GetVolumeControlShift();
	}

	bool WaveSoundChannel::IsConstrainedByLength() const
	{
		return (nrx4 >> 6) & 1;
	}

	uint32_t WaveSoundChannel::GetFrequencyTimerPeriod() const
	{
		uint16_t frequency = (((nrx4 & 0b111) << 8) | nrx3);
		return (2048 - frequency) * 2;
	}

	void WaveSoundChannel::OnFrequencyTimerReachedZero()
	{
		sampleIndex = (sampleIndex + 1) % NUMBER_OF_SAMPLES;
	}

	uint8_t WaveSoundChannel::GetVolumeControlShift() const
	{
		// Bits 5 and 6 of NR32.
		uint8_t volumeControl = (nrx2 >> 5) & 0b11;
		return volumeControl == 0 ? 4 : volumeControl - 1;
	}

	void WaveSoundChannel::OnTrigger()
	{
		SoundChannel::OnTrigger();
		sampleIndex = 0;
	}
}