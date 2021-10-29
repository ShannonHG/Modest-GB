#include <map>
#include "CPU/Timer.hpp"
#include "Utils/DataConversions.hpp"
#include "CPU/Interrupts.hpp"
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

	const std::map<uint8_t, double> TIMER_COUNTER_FREQUENCIES =
	{
		{0, TIMER_CONTROL_MODE_0_FREQ},
		{1, TIMER_CONTROL_MODE_1_FREQ},
		{2, TIMER_CONTROL_MODE_2_FREQ},
		{3, TIMER_CONTROL_MODE_3_FREQ},
	};

	// Timer Registers
	// FF04: DIV - Divider Register (R/W)
	// FF05: TIMA - Timer Counter (R/W)
	// FF06: TIMA - Timer Counter (R/W)
	// FF07: TAC - Timer Control (R/W)

	Timer::Timer(MemoryMap& memoryMap) : memoryMap(memoryMap)
	{
		currentTimerCounterFreq = TIMER_CONTROL_MODE_0_FREQ;
	}

	void Timer::Step(uint32_t cycles)
	{
		dividerRegister += cycles * DIVIDER_INCREMENTS_PER_CYCLE;

		// The divider register is actual only 8 bits, 
		// it's only implemented as a float for precision purposes.
		if (dividerRegister > 255) 
			dividerRegister = 0;

		if (isClockEnabled)
		{
			timerCounter += cycles * currentTimerCounterFreq;

			// Just like the divider register, the timer counter is also only 8 bits.
			if (timerCounter > 255)
			{
				timerCounter = timerModulo;
				RequestInterrupt(memoryMap, InterruptType::Timer);
			}
		}

		if (Logger::IsSystemEventLoggingEnabled)
			PrintStatus();
	}

	uint8_t Timer::Read(uint16_t address)
	{
		switch (address)
		{
		case 0x00:
			return std::floor(dividerRegister);
		case 0x01:
			return std::floor(timerCounter);
		case 0x02:
			return timerModulo;
		case 0x03:
			return timerControl;
		}

		return 0;
	}

	void Timer::Write(uint16_t address, uint8_t value)
	{
		switch (address)
		{
		case 0x00:
			// Writing any value to the divider register resets it to 0.
			dividerRegister = 0;
			break;
		case 0x01:
			timerCounter = value;
			break;
		case 0x02:
			timerModulo = value;
			break;
		case 0x03:
			isClockEnabled = (value & 0b00000100) >> 2;
			currentTimerCounterFreq = TIMER_COUNTER_FREQUENCIES.at(value & 0b00000011);
			timerControl = value;
			break;
		}
	}

	bool Timer::IsAddressAvailable(uint16_t address)
	{
		return address <= 3;
	}

	void Timer::Reset()
	{
		preciseDivider = 0;
		preciseCounter = 0;

		dividerRegister = 0;
		timerCounter = 0;
		timerModulo = 0;
		timerControl = 0;

		isClockEnabled = false;
		currentTimerCounterFreq = 0;
	}

	void Timer::PrintStatus()
	{
		Logger::WriteSystemEvent("(DIV) " + GetHexString8(std::floor(dividerRegister)) +
			" (TIMA) " + GetHexString8(std::floor(timerCounter)) +
			" (TMA) " + GetHexString8(timerModulo) +
			" (TAC) " + GetHexString8(timerControl), TIMER_MESSAGE_HEADER);
	}
}