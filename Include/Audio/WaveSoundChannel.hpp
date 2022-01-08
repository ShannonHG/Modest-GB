#pragma once
#include <cstdint>
#include <vector>
#include "Audio/SoundChannel.hpp"

namespace SHG
{
	class WaveSoundChannel : public SoundChannel
	{
	public:
		void Reset() override;
		WaveSoundChannel();
		void WriteToWavePatternRAM(uint16_t address, uint8_t value);
		uint8_t ReadWavePatternRAM() const;

		void WriteToNRX0(uint8_t value) override;

		uint8_t ReadNRX0() const override;
		uint8_t ReadNRX1() const override;
		uint8_t ReadNRX2() const override;
		uint8_t ReadNRX3() const override;
		uint8_t ReadNRX4() const override;

	protected:
		uint16_t GetLengthTimerPeriod() const override;
		uint16_t GetFrequencyTimerPeriod() const override;
		float GenerateSample() const override;
		bool IsConstrainedByLength() const override;
		void OnFrequencyTimerReachedZero() override;
		void OnTrigger() override;
		ModifierDirection GetVolumeEnvelopeDirection() const;
		uint8_t GetInitialEnvelopeVolume() const;
		uint16_t GetVolumeEnvelopeTimerPeriod() const;

	private:
		uint8_t sampleIndex = 0;
		std::vector<uint8_t> samples;
		uint8_t previousWaveRAMValue = 0;

		uint8_t GetVolumeControlShift() const;
	};
}