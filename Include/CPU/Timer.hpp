#pragma once
#include <vector>
#include "Memory/MemoryMap.hpp"
#include "Memory/DataStorageDevice.hpp"
#include "Memory/Register8.hpp"

namespace SHG
{
	class Timer : public DataStorageDevice
	{
	public:
		Timer(MemoryMap& memoryMap);
		void Update(uint32_t cycles);
		uint8_t GetByte(uint16_t address) override;
		void SetByte(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) override;
	private:
		MemoryMap& memoryMap;

		float preciseDivider = 0;
		float preciseCounter = 0;

		float dividerRegister = 0;
		float timerCounter = 0;
		float timerModulo = 0; 
		float timerControl = 0;

		bool isClockEnabled = false;
		uint16_t currentTimerCounterFreq;

		void PrintStatus();
	};
}