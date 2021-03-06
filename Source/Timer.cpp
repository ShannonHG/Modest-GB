#include <map>
#include <algorithm>
#include "Timer.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Interrupts.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"

namespace ModestGB
{
	const std::string TIMER_MESSAGE_HEADER = "[TIMER]";
	const uint8_t TIMER_OVERFLOW_DELAY_IN_CYCLES = 4;

	// CPU Clock / 16
	const uint8_t CONTROL_MODE_16_BIT_INDEX = 3;
	// CPU Clock / 64
	const uint8_t CONTROL_MODE_64_BIT_INDEX = 5;
	// CPU Clock / 256
	const uint8_t CONTROL_MODE_256_BIT_INDEX = 7;
	// CPU Clock / 1024
	const uint8_t CONTROL_MODE_1024_BIT_INDEX = 9;

	const std::array<uint8_t, 4> TIMER_CONTROL_MODE_BIT_INDEXES =
	{
		CONTROL_MODE_1024_BIT_INDEX,
		CONTROL_MODE_16_BIT_INDEX, 
		CONTROL_MODE_64_BIT_INDEX, 
		CONTROL_MODE_256_BIT_INDEX
	};

	// Timer Registers
	// FF04: DIV - Divider Register (R/W)
	// FF05: TIMA - Timer Counter (R/W)
	// FF06: TIMA - Timer Counter (R/W)
	// FF07: TAC - Timer Control (R/W)

	Timer::Timer(Memory& memoryMap) : memoryMap(memoryMap)
	{

	}

	void Timer::Tick(uint32_t cycles)
	{
		wasCounterReloaded = false;
		for (uint32_t i = 0; i < cycles; i++)
		{
			// If the overflow counter is greater than zero then an overflow is pending.
			if (overflowCounter > 0)
			{
				overflowCounter = std::max(static_cast<int>(overflowCounter - cycles), 0);
				if (overflowCounter == 0)
				{
					timerCounter = timerModulo;
					wasCounterReloaded = true;
					Interrupts::RequestInterrupt(memoryMap, Interrupts::InterruptType::Timer);
				}
			}

			// Incremented every cycle
			SetInternalCounter(internalCounter + 1);

			if (Logger::IsSystemEventLoggingEnabled)
				PrintStatus();
		}
	}

	uint8_t Timer::GetTimerCounter() const
	{
		return timerCounter;
	}

	uint8_t Timer::GetTimerModulo() const
	{
		return timerModulo;
	}

	uint8_t Timer::GetLowerInternalCounter() const
	{
		return internalCounter & 0x00FF;
	}

	uint8_t Timer::GetDividerRegister() const
	{
		// The divider register is the upper 8 bits of the internal counter.
		return internalCounter >> 8;
	}

	uint8_t Timer::GetTimerControlRegister() const
	{
		return (isEnabled << 2) | static_cast<uint8_t>(currentTimerControlMode);
	}

	void Timer::WriteToTimerCounter(uint8_t value)
	{
		// If the timer counter is written to during the cycle that 
		// the modulo is loaded into it, then the write to the counter should be ignored.
		if (wasCounterReloaded) 
			return;

		timerCounter = value;

		// Writing a value to the timer counter cancels any pending overflow.
		overflowCounter = 0;
	}

	void Timer::WriteToTimerModulo(uint8_t value)
	{
		timerModulo = value;

		// When the modulo is written to during the cycle that it is loaded into the timer counter, 
		// then the written value should also be loaded into the counter.
		if (wasCounterReloaded)
			timerCounter = value;
	}

	void Timer::WriteToTimerControlRegister(uint8_t value)
	{
		bool wasPreviouslyEnabled = isEnabled;

		isEnabled = (value & 0b100) >> 2;
		currentTimerControlMode = static_cast<TimerControlMode>(value & 0b11);

		// If the timer was previously enabled, the bit designated as the "timer control bit" is 1,
		// and the timer is no longer enabled, then the timer counter should be incremented.
		if ((wasPreviouslyEnabled && GetCurrentTimerControlBit()) && !isEnabled)
			IncrementTimerCounter();
	}

	void Timer::WriteToDividerRegister(uint8_t value)
	{
		SetInternalCounter(0);
	}

	void Timer::Reset()
	{
		internalCounter = 0;
		timerCounter = 0;
		timerModulo = 0;
		isEnabled = false;
		currentTimerControlMode = TimerControlMode::TIMER_CONTROL_MODE_1024;
	}

	bool Timer::GetCurrentTimerControlBit() const
	{
		// Returns the bit in the internal counter that is responsible for determining when to increment the timer counter (TIMA).
		return (internalCounter >> TIMER_CONTROL_MODE_BIT_INDEXES[static_cast<uint8_t>(currentTimerControlMode)]) & 1;
	}

	void Timer::SetInternalCounter(uint16_t value)
	{
		bool isPrevControlBitEnabled = GetCurrentTimerControlBit();

		internalCounter = value;

		// If the bit designated as the "timer control bit" has changed from 1 to 0, and
		// the timer is enabled, then the timerCounter should be incremented.
		if (isEnabled && (isPrevControlBitEnabled && !GetCurrentTimerControlBit()))
		{
			IncrementTimerCounter();
		}
	}

	void Timer::IncrementTimerCounter()
	{
		if (timerCounter == 255)
		{
			// When the timer counter overflows, there is a 4 cycle delay before
			// the modulo will is loaded into it, and the timer interrupt is requested. 
			// During these 4 cycles, the value in the timer counter is 0.
			timerCounter = 0;
			overflowCounter = TIMER_OVERFLOW_DELAY_IN_CYCLES;
		}
		else
		{
			timerCounter++;
		}
	}

	void Timer::PrintStatus() const
	{
		Logger::WriteSystemEvent("(DIV) " + Convert::GetHexString8(static_cast<uint8_t>(std::floor(GetDividerRegister()))) +
			" (TIMA) " + Convert::GetHexString8(static_cast<uint8_t>(std::floor(timerCounter))) +
			" (TMA) " + Convert::GetHexString8(timerModulo) +
			" (TAC) " + Convert::GetHexString8(GetTimerControlRegister()), TIMER_MESSAGE_HEADER);
	}
}