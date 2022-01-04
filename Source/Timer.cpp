#include <map>
#include "Timer.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Interrupts.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"

namespace SHG
{
	const std::string TIMER_MESSAGE_HEADER = "[TIMER]";

	const std::map <TimerControlMode, uint8_t> TIMER_CONTROL_MODE_BIT_INDEXES =
	{
		{TimerControlMode::TIMER_CONTROL_MODE_16, 3},
		{TimerControlMode::TIMER_CONTROL_MODE_64, 5},
		{TimerControlMode::TIMER_CONTROL_MODE_256, 7},
		{TimerControlMode::TIMER_CONTROL_MODE_1024, 9},
	};

	// Timer Registers
	// FF04: DIV - Divider Register (R/W)
	// FF05: TIMA - Timer Counter (R/W)
	// FF06: TIMA - Timer Counter (R/W)
	// FF07: TAC - Timer Control (R/W)

	// TODO: Handle additional obscure behavior.
	Timer::Timer(Memory& memoryMap) : memoryMap(memoryMap)
	{

	}

	void Timer::Tick(uint32_t cycles)
	{
		// Incremented every cycle
		SetInternalCounter(internalCounter + cycles);

		if (Logger::IsSystemEventLoggingEnabled)
			PrintStatus();
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
		return (isClockEnabled << 2) | static_cast<uint8_t>(currentTimerControlMode);
	}

	void Timer::WriteToTimerCounter(uint8_t value)
	{
		timerCounter = value;
	}

	void Timer::WriteToTimerModulo(uint8_t value)
	{
		timerModulo = value;
	}

	void Timer::WriteToTimerControlRegister(uint8_t value)
	{
		isClockEnabled = (value & 0b100) >> 2;
		currentTimerControlMode = static_cast<TimerControlMode>(value & 0b11);
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
		isClockEnabled = false;
		currentTimerControlMode = TimerControlMode::TIMER_CONTROL_MODE_1024;
	}

	bool Timer::GetCurrentTimerControlBit() const
	{
		return (internalCounter >> TIMER_CONTROL_MODE_BIT_INDEXES.at(currentTimerControlMode)) & 1;
	}

	void Timer::SetInternalCounter(uint16_t value)
	{
		bool isPrevControlBitEnabled = GetCurrentTimerControlBit();

		internalCounter = value;

		// If the bit designated as the "timer control bit" has changed from 1 to 0, then the timerCounter,
		// should be incremented.
		if (isClockEnabled && (isPrevControlBitEnabled && !GetCurrentTimerControlBit()))
		{
			if (timerCounter == 255)
			{
				timerCounter = timerModulo;
				RequestInterrupt(memoryMap, InterruptType::Timer);
			}
			else
			{
				timerCounter++;
			}
		}
	}

	void Timer::PrintStatus() const
	{
		Logger::WriteSystemEvent("(DIV) " + GetHexString8(static_cast<uint8_t>(std::floor(GetDividerRegister()))) +
			" (TIMA) " + GetHexString8(static_cast<uint8_t>(std::floor(timerCounter))) +
			" (TMA) " + GetHexString8(timerModulo) +
			" (TAC) " + GetHexString8(GetTimerControlRegister()), TIMER_MESSAGE_HEADER);
	}
}