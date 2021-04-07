#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "Globals.hpp"

namespace SHG
{
	class MemoryBankController
	{
	public:
		void AttachRAM(std::vector<uint8_t>& ram);
		void AttachROM(std::vector<uint8_t>& rom);

		virtual bool TryGetByte(uint16_t address, uint8_t& outValue) = 0;
		virtual bool TrySetByte(uint16_t address, uint8_t value) = 0;

		/// <summary>
		/// Calculates the physical ROM address using the RAM bank number, and distance from the beginning of 
		/// the ROM bank's address (in the virtual address range) to the target virtual address
		/// </summary>
		static uint32_t CalculatePhysicalROMAddress(uint16_t romBankNumber, uint16_t virtualAddressRangeStart, uint16_t targetVirtualAddress);

		/// <summary>
		/// Calculates the physical RAM address using the RAM bank number, and distance from the beginning of 
		/// the RAM bank's address (in the virtual address range) to the target virtual address
		/// </summary>
		static uint32_t CalculatePhysicalRAMAddress(uint8_t ramBankNumber, uint16_t virtualAddressRangeStart, uint16_t targetVirtualAddress);

	protected:

		std::vector<uint8_t>* ram;
		std::vector<uint8_t>* rom;

		virtual std::string GetName() = 0;
		void WriteMissingROMMessage();
		void WriteMissingRAMMessage();
		void WriteInvalidROMAccesMessage(uint16_t address);
		void WriteInvalidRAMAccesMessage(uint16_t address);

		bool TrySetByteInRAM(uint32_t address, uint8_t value);
		bool TryGetByteFromRAM(uint32_t address, uint8_t& outValue);
		bool TrySetByteInROM(uint32_t address, uint8_t value);
		bool TryGetByteFromROM(uint32_t address, uint8_t& outValue);
	};
}