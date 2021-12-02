#include <map>
#include "CPU/Timer.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Interrupts.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"

namespace SHG
{
	const std::string TIMER_MESSAGE_HEADER = "[TIMER]";

	const uint16_t DIVIDER_INCREMENTS_PER_SECOND = 16384;
	const double DIVIDER_INCREMENTS_PER_CYCLE = DIVIDER_INCREMENTS_PER_SECOND / (double)GB_CLOCK_SPEED;

	const double TIMER_CONTROL_MODE_0_FREQ = (GB_CLOCK_SPEED / 1024.0) / (double)GB_CLOCK_SPEED;
	const double TIMER_CONTROL_MODE_1_FREQ = (GB_CLOCK_SPEED / 16.0) / (double)GB_CLOCK_SPEED;
	const double TIMER_CONTROL_MODE_2_FREQ = (GB_CLOCK_SPEED / 64.0) / (double)GB_CLOCK_SPEED;
	const double TIMER_CONTROL_MODE_3_FREQ = (GB_CLOCK_SPEED / 256.0) / (double)GB_CLOCK_SPEED;

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
	Timer::Timer(MemoryMap& memoryMap) : memoryMap(memoryMap)
	{

	}

	void Timer::Step(uint32_t cycles)
	{
		// Incremented every cycle
		SetInternalCounter(internalCounter + cycles);

		if (Logger::IsSystemEventLoggingEnabled)
			PrintStatus();
	}

	uint8_t Timer::Read(uint16_t address) const
	{
		switch (address)
		{
		case 0x00:
			return internalCounter & 0x00FF;
		case 0x01:
			return GetDividerRegister();
		case 0x02:
			return timerCounter;
		case 0x03:
			return timerModulo;
		case 0x04:
			return GetTimerControlRegister();
		}

		return 0;
	}

	void Timer::Write(uint16_t address, uint8_t value)
	{
		switch (address)
		{
		case 0x00:
			break;
		case 0x01:
			// Writing any value to this address resets the internal counter (and divider register).
			SetInternalCounter(0);
			break;
		case 0x02:
			timerCounter = value;
			break;
		case 0x03:
			timerModulo = value;
			break;
		case 0x04:
			isClockEnabled = (value & 0b100) >> 2;
			currentTimerControlMode = static_cast<TimerControlMode>(value & 0b11);
			break;
		}
	}

	bool Timer::IsAddressAvailable(uint16_t address) const
	{
		return address <= 4;
	}

	void Timer::Reset()
	{
		internalCounter = 0;
		timerCounter = 0;
		timerModulo = 0;
		isClockEnabled = false;
		currentTimerControlMode = TimerControlMode::TIMER_CONTROL_MODE_1024;
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
		Logger::WriteSystemEvent("(DIV) " + GetHexString8(std::floor(GetDividerRegister())) +
			" (TIMA) " + GetHexString8(std::floor(timerCounter)) +
			" (TMA) " + GetHexString8(timerModulo) +
			" (TAC) " + GetHexString8(GetTimerControlRegister()), TIMER_MESSAGE_HEADER);
	}
}