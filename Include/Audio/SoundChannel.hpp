#pragma once
#include <cstdint>
#include <functional>
#include "Audio/AudioTimer.hpp"

namespace SHG
{
	class SoundChannel
	{
	public:
		SoundChannel();

		void TickFrequencyTimer();
		void TickVolumeEnvelopeTimer();
		void TickLengthControlTimer();

		bool IsEnabled();

		void Reset();
		float GetSample() const;

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

		uint8_t volume = 0;

		uint8_t nrx0 = 0;
		uint8_t nrx1 = 0;
		uint8_t nrx2 = 0;
		uint8_t nrx3 = 0;
		uint8_t nrx4 = 0;

		void DisableVolumeEnvelope();
		void ReloadLengthTimer();

		virtual void OnTrigger();
		virtual void OnFrequencyTimerReachedZero();

		virtual ModifierDirection GetVolumeEnvelopeDirection() const;
		virtual uint8_t GetInitialEnvelopeVolume() const;
		virtual bool IsConstrainedByLength() const = 0;
		virtual float GenerateSample() const = 0;

		virtual uint32_t GetFrequencyTimerPeriod() const = 0;
		virtual uint32_t GetLengthTimerPeriod() const = 0;
		virtual uint32_t GetVolumeEnvelopeTimerPeriod() const;

	private:
		AudioTimer frequencyTimer;
		AudioTimer lengthTimer;
		AudioTimer envelopeTimer;
	};
}