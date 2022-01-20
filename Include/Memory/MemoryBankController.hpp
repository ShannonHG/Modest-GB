#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <functional>
#include "Utils/DataConversions.hpp"
#include "Utils/MemoryUtils.hpp"
#include "Memory/Memory.hpp"

namespace ModestGB
{
	using RAMWriteCallback = std::function<void(uint16_t, uint8_t)>;

	class MemoryBankController : public Memory
	{
	public:
		static const uint16_t ROM_BANK_SIZE;
		static const uint16_t RAM_BANK_SIZE;

		void SetRAMWriteCallback(RAMWriteCallback callback);
			
		void AttachRAM(std::vector<uint8_t>& ram);
		void AttachROM(std::vector<uint8_t>& rom);

		void Reset() override;
		virtual uint8_t Read(uint16_t address) const override = 0;
		virtual void Write(uint16_t address, uint8_t value) override = 0;

		// Calculates the physical ROM address using the RAM bank number, and distance from the beginning of 
		// the ROM bank's address (in the virtual address range) to the target virtual address
		static uint32_t CalculatePhysicalROMAddress(uint16_t romBankNumber, uint16_t virtualAddressRangeStart, uint16_t targetVirtualAddress);

		// Calculates the physical RAM address using the RAM bank number, and distance from the beginning of 
		// the RAM bank's address (in the virtual address range) to the target virtual address
		static uint32_t CalculatePhysicalRAMAddress(uint8_t ramBankNumber, uint16_t virtualAddressRangeStart, uint16_t targetVirtualAddress);

	protected:
		RAMWriteCallback ramWriteCallback;

		std::vector<uint8_t>* ram{};
		std::vector<uint8_t>* rom{};

		virtual std::string GetName() const = 0;
		void PrintMissingROMMessage() const;
		void PrintMissingRAMMessage() const;
		void PrintInvalidROMAccessMessage(uint32_t address) const;
		void PrintInvalidRAMAccesMessage(uint32_t address) const;

		void WriteToRAM(uint32_t address, uint8_t value);
		uint8_t ReadFromRAM(uint32_t address) const;
		void WriteToROM(uint32_t address, uint8_t value);
		uint8_t ReadFromROM(uint32_t address) const;

		std::string GetMessageHeader() const;
	};
}