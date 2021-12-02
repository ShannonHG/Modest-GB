#pragma once
#include <vector>
#include "Memory/MemoryMap.hpp"
#include "Memory/DataStorageDevice.hpp"
#include "Memory/Register8.hpp"

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

	class Timer : public DataStorageDevice
	{
	public:
		Timer(MemoryMap& memoryMap);
		void Step(uint32_t cycles);
		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;
	private:
		MemoryMap& memoryMap;

		uint16_t internalCounter = 0;
		uint8_t timerCounter = 0;
		uint8_t timerModulo = 0; 
		TimerControlMode currentTimerControlMode = TimerControlMode::TIMER_CONTROL_MODE_1024;
		bool isClockEnabled = false;

		void PrintStatus() const;
		uint8_t GetDividerRegister() const;
		uint8_t GetTimerControlRegister() const;
		bool GetCurrentTimerControlBit() const;
		void SetInternalCounter(uint16_t value);
	};
}