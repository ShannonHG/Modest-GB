#include "Audio/WaveSoundChannel.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace ModestGB
{
	const uint16_t WAVE_PATTERN_RAM_START_ADDRESS = 0xFF30;
	const uint16_t WAVE_PATTERN_RAM_END_ADDRESS = 0xFF3F;
	const uint8_t NUMBER_OF_SAMPLES = 32;

	WaveSoundChannel::WaveSoundChannel() : SoundChannel()
	{
		samples = std::vector<uint8_t>(NUMBER_OF_SAMPLES);
	}

	void WaveSoundChannel::Reset()
	{
		SoundChannel::Reset();

		sampleIndex = 0;
		std::fill(samples.begin(), samples.end(), 0);
	}

	void WaveSoundChannel::WriteToNRX0(uint8_t value)
	{
		SoundChannel::WriteToNRX0(value);

		isEnabled = nrx0.Read(7);
	}

	void WaveSoundChannel::WriteToWavePatternRAM(uint16_t address, uint8_t value)
	{
		if (address < WAVE_PATTERN_RAM_START_ADDRESS || address > WAVE_PATTERN_RAM_END_ADDRESS)
		{
			Logger::WriteError("Wave pattern RAM address is out of bounds: " + Convert::GetHexString16(address));
			return;
		}

		samples[address - WAVE_PATTERN_RAM_START_ADDRESS] = (value >> 4) & 0b1111;
		samples[(address - WAVE_PATTERN_RAM_START_ADDRESS) + 1] = value & 0b1111;
	}

	uint8_t WaveSoundChannel::ReadNRX0() const
	{
		return SoundChannel::ReadNRX0() | 0x7F;
	}

	uint8_t WaveSoundChannel::ReadNRX1() const
	{
		return SoundChannel::ReadNRX1() | 0xFF;
	}

	uint8_t WaveSoundChannel::ReadNRX2() const
	{
		return SoundChannel::ReadNRX2() | 0x9F;
	}

	uint8_t WaveSoundChannel::ReadNRX3() const
	{
		return SoundChannel::ReadNRX3() | 0xFF;
	}

	uint8_t WaveSoundChannel::ReadNRX4() const
	{
		return SoundChannel::ReadNRX4() | 0xBF;
	}

	uint8_t WaveSoundChannel::ReadWavePatternRAM() const
	{
		return isEnabled ? samples[sampleIndex] : 0xFF;
	}

	uint16_t WaveSoundChannel::GetLengthTimerPeriod() const
	{
		return nrx1.Read();
	}

	float WaveSoundChannel::GenerateSample() const
	{
		return static_cast<float>(samples[sampleIndex] >> GetVolumeControlShift());
	}

	bool WaveSoundChannel::IsConstrainedByLength() const
	{
		return nrx4.Read(6);
	}

	uint16_t WaveSoundChannel::GetFrequencyTimerPeriod() const
	{
		uint16_t frequency = (nrx4.Read(0, 2) << 8) | nrx3.Read();
		return (2048 - frequency) * 2;
	}

	void WaveSoundChannel::OnFrequencyTimerReachedZero()
	{
		SoundChannel::OnFrequencyTimerReachedZero();

		sampleIndex = (sampleIndex + 1) % NUMBER_OF_SAMPLES;
	}

	uint8_t WaveSoundChannel::GetVolumeControlShift() const
	{
		uint8_t volumeControl = nrx2.Read(5, 6);
		return volumeControl == 0 ? 4 : volumeControl - 1;
	}

	void WaveSoundChannel::OnTrigger()
	{
		SoundChannel::OnTrigger();
		sampleIndex = 0;
	}

	uint8_t WaveSoundChannel::GetInitialEnvelopeVolume() const
	{
		return 0;
	}

	uint16_t WaveSoundChannel::GetVolumeEnvelopeTimerPeriod() const
	{
		return 0;
	}

	SoundChannel::ModifierDirection WaveSoundChannel::GetVolumeEnvelopeDirection() const
	{
		return ModifierDirection::Increase;
	}
}