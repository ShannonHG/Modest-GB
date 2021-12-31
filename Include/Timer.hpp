#pragma once
#include <vector>
#include "Memory/Memory.hpp"

namespace SHG
{
	enum class TimerControlMode
	{
		// CPU Clock / 1024
		TIMER_CONTROL_MODE_1024 = 0,
		// CPU Clock / 16
		TIMER_CONTROL_MODE_16 = 1,
		// CPU Clock / 64
		TIMER_CONTROL_MODE_64 = 2,
		// CPU Clock / 256
		TIMER_CONTROL_MODE_256 = 3
	};

	class Timer
	{
	public:
		Timer(Memory& memory);
		void Tick(uint32_t cycles);
		uint8_t GetTimerCounter() const;
		uint8_t GetTimerModulo() const;
		uint8_t GetTimerControlRegister() const;
		uint8_t GetDividerRegister() const;
		uint8_t GetLowerInternalCounter() const;

		void WriteToTimerCounter(uint8_t value);
		void WriteToTimerModulo(uint8_t value);
		void WriteToTimerControlRegister(uint8_t value);
		void WriteToDividerRegister(uint8_t value);

		void Reset();
	private:
		Memory& memoryMap;

		uint16_t internalCounter = 0;
		uint8_t timerCounter = 0;
		uint8_t timerModulo = 0; 
		TimerControlMode currentTimerControlMode = TimerControlMode::TIMER_CONTROL_MODE_1024;
		bool isClockEnabled = false;

		bool GetCurrentTimerControlBit() const;
		void SetInternalCounter(uint16_t value);

		void PrintStatus() const;
	};
}