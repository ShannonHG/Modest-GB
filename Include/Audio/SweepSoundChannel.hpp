#pragma once
#include "Audio/ToneSoundChannel.hpp"

namespace SHG
{
	class SweepSoundChannel : public ToneSoundChannel
	{
	public:
		SweepSoundChannel();
		void TickSweepTimer();

	protected:
		void OnTrigger() override;

	private:
		bool isSweepEnabled = false;

		AudioTimer sweepTimer;
		uint16_t shadowFrequency = 0;

		uint32_t CalculateShadowFrequency() const;
		bool PerformOverflowCheck(uint32_t newFrequency);
		void UpdateFrequency(uint16_t newFrequency);

		uint8_t GetSweepPeriod() const;
		ModifierDirection GetSweepDirection() const;
		uint8_t GetSweepShift() const;
	};
}