#pragma once
#include <string>
#include <map>
#include <vector>
#include "Utils/Arithmetic.hpp"
#include "Memory/MemoryBankController.hpp"

namespace SHG
{
	enum class MemoryBankControllerType
	{
		None,
		MBC1,
		MBC2,
		MBC3,
		MBC5,
		MBC6,
		MBC7,
		MMM01,
		HuC1,
		HuC3
	};

	class Cartridge : public DataStorageDevice
	{
	public:
		bool Load(const std::string& romFilePath);
		bool Load(const std::vector<uint8_t>& romData);

		MemoryBankControllerType GetMemoryBankControllerType();
		uint32_t GetROMSize();
		uint32_t GetRAMSize();
		bool IsROMLoaded();

		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;
	private:
		bool isROMLoaded = false;

		std::unique_ptr<MemoryBankController> memoryBankController;
		MemoryBankControllerType memoryBankControllerType;

		std::vector<uint8_t> ram;
		std::vector<uint8_t> rom;
		std::string title;

		void InitializeMemoryBankController(uint8_t byte);
		void DecodeROMSize(uint8_t byte);
		void InitializeRAM(uint8_t byte);
		bool IsRAMAddress(uint16_t address) const;
	};
}