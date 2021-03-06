#include <string>
#include "Audio/NoiseSoundChannel.hpp"
#include "Logger.hpp"

namespace ModestGB
{
	NoiseSoundChannel::NoiseSoundChannel() : SoundChannel()
	{
	}

	void NoiseSoundChannel::Reset()
	{
		SoundChannel::Reset();

		shiftRegister.Fill(false);
	}

	void NoiseSoundChannel::WriteToNRX1(uint8_t value)
	{
		SoundChannel::WriteToNRX1(value);

		ReloadLengthTimer();
	}

	void NoiseSoundChannel::WriteToNRX2(uint8_t value)
	{
		SoundChannel::WriteToNRX2(value);

		// If the upper 5 bits of NR42 are 0, then this channel is disabled.
		if (nrx2.Read(3, 7) == 0)
			isEnabled = false;

		// If the period is 0, then stop the volume envelope.
		if (GetVolumeEnvelopeTimerPeriod() == 0)
			DisableVolumeEnvelope();
	}

	uint8_t NoiseSoundChannel::ReadNRX0() const
	{
		return SoundChannel::ReadNRX0() | 0xFF;
	}

	uint8_t NoiseSoundChannel::ReadNRX1() const
	{
		return SoundChannel::ReadNRX1() | 0xFF;
	}

	uint8_t NoiseSoundChannel::ReadNRX4() const
	{
		return SoundChannel::ReadNRX4() | 0xBF;
	}

	uint16_t NoiseSoundChannel::GetFrequencyTimerPeriod() const
	{
		uint8_t divisorCode = nrx3.Read(0, 2);
		return (divisorCode == 0 ? 8 : divisorCode * 16) << GetFrequencyShift();
	}

	void NoiseSoundChannel::OnFrequencyTimerReachedZero()
	{
		SoundChannel::OnFrequencyTimerReachedZero();

		uint8_t xorResult = shiftRegister.Read(0) ^ shiftRegister.Read(1);
		shiftRegister.Write(shiftRegister.Read() >> 1);
		shiftRegister.SetBit(14, xorResult);

		if (GetShiftRegisterSize() == 7)
			shiftRegister.SetBit(6, xorResult);
	}

	float NoiseSoundChannel::GenerateSample() const
	{
		return static_cast<float>(((~shiftRegister.Read()) & 1) * volume);
	}

	uint16_t NoiseSoundChannel::GetLengthTimerPeriod() const
	{
		return nrx1.Read(0, 5);
	}

	SoundChannel::ModifierDirection NoiseSoundChannel::GetVolumeEnvelopeDirection() const
	{
		return nrx2.Read(3) ?  ModifierDirection::Increase : ModifierDirection::Decrease;
	}

	uint8_t NoiseSoundChannel::GetInitialEnvelopeVolume() const
	{
		return nrx2.Read(4, 7);
	}

	uint16_t NoiseSoundChannel::GetVolumeEnvelopeTimerPeriod() const
	{
		return nrx2.Read(0, 2);
	}

	bool NoiseSoundChannel::IsConstrainedByLength() const
	{
		return nrx4.Read(6);
	}

	void NoiseSoundChannel::OnTrigger()
	{
		SoundChannel::OnTrigger();

		shiftRegister.Fill(1);
	}

	uint8_t NoiseSoundChannel::GetShiftRegisterSize() const
	{
		return nrx3.Read(3);
	}

	uint8_t NoiseSoundChannel::GetFrequencyShift() const
	{
		return nrx3.Read(4, 7) + 1;
	}
}