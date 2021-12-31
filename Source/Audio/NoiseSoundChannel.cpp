#include <string>
#include "Audio/NoiseSoundChannel.hpp"
#include "Logger.hpp"

namespace SHG
{
	NoiseSoundChannel::NoiseSoundChannel() : SoundChannel()
	{
	}

	void NoiseSoundChannel::WriteToNRX1(uint8_t value)
	{
		SoundChannel::WriteToNRX1(value);

		ReloadLengthTimer();
	}

	void NoiseSoundChannel::WriteToNRX2(uint8_t value)
	{
		SoundChannel::WriteToNRX2(value);

		// If the period is 0, then stop the volume envelope.
		if (GetVolumeEnvelopeTimerPeriod() == 0)
			DisableVolumeEnvelope();
	}

	uint32_t NoiseSoundChannel::GetFrequencyTimerPeriod() const
	{
		uint8_t divisorCode = nrx3 & 0b111;
		return (divisorCode == 0 ? 8 : divisorCode * 16) << GetFrequencyShift();
	}

	void NoiseSoundChannel::OnFrequencyTimerReachedZero()
	{
		uint8_t xorResult = shiftRegister.GetBit(0) ^ shiftRegister.GetBit(1);
		shiftRegister.Write(shiftRegister.Read() >> 1);
		shiftRegister.SetBit(14, xorResult);

		if (GetShiftRegisterSize() == 7)
			shiftRegister.SetBit(6, xorResult);
	}

	float NoiseSoundChannel::GenerateSample() const
	{
		return ((~shiftRegister.Read()) & 1) * volume;
	}

	uint32_t NoiseSoundChannel::GetLengthTimerPeriod() const
	{
		return nrx1 & 0b111111;
	}

	SoundChannel::ModifierDirection NoiseSoundChannel::GetVolumeEnvelopeDirection() const
	{
		return ((nrx2 >> 3) & 1) == 0 ? ModifierDirection::Decrease : ModifierDirection::Increase;
	}

	uint8_t NoiseSoundChannel::GetInitialEnvelopeVolume() const
	{
		return (nrx2 >> 4) & 0b1111;
	}

	uint32_t NoiseSoundChannel::GetVolumeEnvelopeTimerPeriod() const
	{
		return nrx2 & 0b111;
	}

	bool NoiseSoundChannel::IsConstrainedByLength() const
	{
		return (nrx4 >> 6) & 1;
	}

	void NoiseSoundChannel::OnTrigger()
	{
		SoundChannel::OnTrigger();

		shiftRegister.Write(0b1111111111111111);
	}

	uint8_t NoiseSoundChannel::GetShiftRegisterSize() const
	{
		return (nrx3 >> 3) & 1;
	}

	uint8_t NoiseSoundChannel::GetFrequencyShift() const
	{
		return ((nrx3 >> 4) & 0b1111) + 1;
	}
}