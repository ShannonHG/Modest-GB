#pragma once
#include "Audio/SoundChannel.hpp"
#include "Memory/Register16.hpp"

namespace SHG
{
	class NoiseSoundChannel : public SoundChannel
	{
	public:
		NoiseSoundChannel();

		void WriteToNRX1(uint8_t value) override;
		void WriteToNRX2(uint8_t value) override;

	protected:
		float GenerateSample() const override;
		void OnTrigger() override;
		uint32_t GetFrequencyTimerPeriod() const override;
		uint32_t GetLengthTimerPeriod() const override;
		ModifierDirection GetVolumeEnvelopeDirection() const override;
		uint32_t GetVolumeEnvelopeTimerPeriod() const override;
		bool IsConstrainedByLength() const override;
		void OnFrequencyTimerReachedZero() override;
		uint8_t GetInitialEnvelopeVolume() const override;

	private:
		Register16 shiftRegister;

		uint8_t GetShiftRegisterSize() const;
		uint8_t GetFrequencyShift() const;
	};
}