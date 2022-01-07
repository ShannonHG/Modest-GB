#pragma once
#include "Audio/SoundChannel.hpp"
#include "Memory/Register16.hpp"

namespace SHG
{
	class NoiseSoundChannel : public SoundChannel
	{
	public:
		NoiseSoundChannel();

		void Reset() override;
		void WriteToNRX1(uint8_t value) override;
		void WriteToNRX2(uint8_t value) override;

		uint8_t ReadNRX0() const override;
		uint8_t ReadNRX1() const override;
		uint8_t ReadNRX2() const override;
		uint8_t ReadNRX3() const override;
		uint8_t ReadNRX4() const override;

	protected:
		float GenerateSample() const override;
		void OnTrigger() override;
		uint16_t GetFrequencyTimerPeriod() const override;
		uint16_t GetLengthTimerPeriod() const override;
		ModifierDirection GetVolumeEnvelopeDirection() const override;
		uint16_t GetVolumeEnvelopeTimerPeriod() const override;
		bool IsConstrainedByLength() const override;
		void OnFrequencyTimerReachedZero() override;
		uint8_t GetInitialEnvelopeVolume() const override;

	private:
		Register16 shiftRegister;

		uint8_t GetShiftRegisterSize() const;
		uint8_t GetFrequencyShift() const;
	};
}