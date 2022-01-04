#pragma once
#include <cstdint>
#include <functional>
#include "Audio/AudioTimer.hpp"
#include "Memory/Register8.hpp"

namespace SHG
{
	class SoundChannel
	{
	public:
		SoundChannel();

		void TickFrequencyTimer();
		void TickVolumeEnvelopeTimer();
		void TickLengthControlTimer();

		bool IsEnabled() const;

		void EnableSoundController();
		void DisableSoundController();
		float GetSample() const;

		virtual void Reset();

		virtual void WriteToNRX0(uint8_t value);
		virtual void WriteToNRX1(uint8_t value);
		virtual void WriteToNRX2(uint8_t value);
		virtual void WriteToNRX3(uint8_t value);
		virtual void WriteToNRX4(uint8_t value);

		virtual uint8_t ReadNRX0() const;
		virtual uint8_t ReadNRX1() const;
		virtual uint8_t ReadNRX2() const;
		virtual uint8_t ReadNRX3() const;
		virtual uint8_t ReadNRX4() const;

	protected:
		enum class ModifierDirection
		{
			Increase = 1,
			Decrease = -1
		};

		bool isEnabled = false;
		bool isSoundControllerEnabled = true;

		uint8_t volume = 0;

		Register8 nrx0;
		Register8 nrx1;
		Register8 nrx2;
		Register8 nrx3;
		Register8 nrx4;

		void DisableVolumeEnvelope();
		void ReloadLengthTimer();

		virtual void OnTrigger();
		virtual void OnFrequencyTimerReachedZero();

		virtual ModifierDirection GetVolumeEnvelopeDirection() const = 0;
		virtual uint8_t GetInitialEnvelopeVolume() const = 0;
		virtual bool IsConstrainedByLength() const = 0;
		virtual float GenerateSample() const = 0;

		virtual uint32_t GetFrequencyTimerPeriod() const = 0;
		virtual uint32_t GetLengthTimerPeriod() const = 0;
		virtual uint32_t GetVolumeEnvelopeTimerPeriod() const = 0;

	private:
		AudioTimer frequencyTimer;
		AudioTimer lengthTimer;
		AudioTimer envelopeTimer;
	};
}