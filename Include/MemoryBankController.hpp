#pragma once
#include <cstdint>
#include <vector>

namespace SHG
{
	class MemoryBankController
	{
	public:
		bool IsRAMEnabled();
		uint8_t GetROMBankNumber();

		void AttachRAM(std::vector<uint8_t>& ram);
		void AttachROM(std::vector<uint8_t>& rom);

		virtual uint8_t GetByte(uint16_t address) = 0;
		virtual void SetByte(uint16_t address, uint8_t value) = 0;

	protected:
		bool isRAMEnabled;
		uint8_t romBankNumber;
		std::vector<uint8_t>* ram;
		std::vector<uint8_t>* rom;
	};
}