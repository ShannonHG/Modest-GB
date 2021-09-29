#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "Common/DataConversions.hpp"
#include "Common/MemoryUnits.hpp"
#include "Memory/DataStorageDevice.hpp"

namespace SHG
{
	class MemoryBankController : public DataStorageDevice
	{
	public:
		static const uint16_t ROM_BANK_SIZE;
		static const uint16_t RAM_BANK_SIZE;

		void AttachRAM(std::vector<uint8_t>& ram);
		void AttachROM(std::vector<uint8_t>& rom);

		virtual uint8_t GetByte(uint16_t address) override = 0;
		virtual void SetByte(uint16_t address, uint8_t value) override = 0;
		virtual bool IsAddressAvailable(uint16_t address) override = 0;

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

		std::vector<uint8_t>* ram{};
		std::vector<uint8_t>* rom{};

		virtual std::string GetName() = 0;
		void WriteMissingROMMessage();
		void WriteMissingRAMMessage();
		void WriteInvalidROMAccesMessage(uint16_t address);
		void WriteInvalidRAMAccesMessage(uint16_t address);

		void SetByteInRAM(uint32_t address, uint8_t value);
		uint8_t GetByteFromRAM(uint32_t address);
		void SetByteInROM(uint32_t address, uint8_t value);
		uint8_t GetByteFromROM(uint32_t address);
	};
}