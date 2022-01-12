#pragma once
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "Utils/Arithmetic.hpp"
#include "Memory/MemoryBankController.hpp"

namespace SHG
{
	enum class SavedDataSearchType
	{
		ROM_DIRECTORY, 
		EMULATOR_DIRECTORY
	};

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

	class Cartridge : public Memory
	{
	public:
		bool Load(const std::string& romFilePath);

		MemoryBankControllerType GetMemoryBankControllerType();
		uint32_t GetROMSize();
		uint32_t GetRAMSize();
		bool IsROMLoaded();

		void SetSavedDataSearchType(SavedDataSearchType searchType);
		SavedDataSearchType GetSavedDataSearchType() const;

		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		void Reset() override;

	private:
		std::fstream savedDataStream;
		std::string savedDataPath;
		bool isROMLoaded = false;
		SavedDataSearchType savedDataSearchType = SavedDataSearchType::ROM_DIRECTORY;

		std::unique_ptr<MemoryBankController> memoryBankController;
		MemoryBankControllerType memoryBankControllerType;

		std::vector<uint8_t> ram;
		std::vector<uint8_t> rom;
		std::string title;

		bool Load(const std::vector<uint8_t>& romData);

		void InitializeMemoryBankController(uint8_t byte);
		void DecodeROMSize(uint8_t byte);
		void InitializeRAM(uint8_t byte);
		bool IsRAMAddress(uint16_t address) const;
		void WriteUnsupportedMBCMesage();

		void OpenSavedDataFile();

		void OnRAMWrite(uint16_t address, uint8_t value);
	};
}