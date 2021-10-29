#include <fstream>
#include <cmath>
#include <cassert>
#include "Logger.hpp"
#include "Memory/Cartridge.hpp"
#include "Logger.hpp"
#include "Memory/MBC1.hpp"

namespace SHG
{
	const std::string CARTRIDGE_LOG_HEADER = "[CART]";

	bool Cartridge::LoadFromFile(std::string romFilePath)
	{
		title.clear();

		std::ifstream file(romFilePath, std::ios::binary);

		if (!file.is_open()) 
			return false;

		// Buffer for storing the current byte
		char buffer[1];

		int address = 0;

		while (!file.eof())
		{
			file.read(buffer, 1);

			DecodeROMByte(buffer[0], address);

			address++;
		}

		file.close();

		InitMBC();

		Logger::WriteInfo("ROM title: " + title, CARTRIDGE_LOG_HEADER);

		isROMLoaded = true;
		return true;
	}

	bool Cartridge::LoadFromMemory(std::vector<uint8_t>& data)
	{
		uint16_t address = 0;
		for (uint8_t& byte : data)
		{
			DecodeROMByte(byte, address);

			address++;
		}

		InitMBC();

		isROMLoaded = true;
		return true;
	}

	void Cartridge::Reset()
	{
		isROMLoaded = false;

		memoryBankController.reset();
		memoryBankControllerType = MemoryBankControllerType::None;

		ram.clear();
		rom.clear();
	}

	void Cartridge::InitMBC()
	{
		if (memoryBankController != NULL)
		{
			Logger::WriteInfo("Initializing MBC", CARTRIDGE_LOG_HEADER);
			if (GetRAMSize() > 0)
			{
				Logger::WriteInfo("Attaching RAM to MBC", CARTRIDGE_LOG_HEADER);
				memoryBankController->AttachRAM(ram);
			}
			if (GetROMSize() > 0)
			{
				Logger::WriteInfo("Attaching ROM to MBC", CARTRIDGE_LOG_HEADER);
				memoryBankController->AttachROM(rom);
			}
		}
	}

	void Cartridge::DecodeROMByte(uint8_t byte, uint16_t address)
	{
		if (address >= CH_TITLE_START_ADDRESS && address <= CH_TITLE_END_ADDRESS)
			title += (char)byte;

		switch (address)
		{
		case CH_CGB_FLAG_ADDRESS:
			// If the most significant bit is enabled, then CGB mode is enabled.
			if ((byte & 0b10000000) == 0b10000000) 
				Logger::WriteInfo("CGB mode is enabled", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MEMORY_BANK_TYPE_ADDRESS:
			DecodeCartridgeType(byte);
			break;
		case CH_ROM_SIZE_ADDRESS:
			DecodeROMSize(byte);
			break;
		case CH_RAM_SIZE_ADDRESS:
			DecodeRAMSize(byte);
			break;
		}

		rom.push_back(byte);
	}

	void Cartridge::DecodeCartridgeType(uint8_t byte)
	{
		switch (byte)
		{
		case CH_ROM_ONLY_CODE:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: 'ROM ONLY'", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MBC1_CODE:
		case CH_MBC1_RAM_CODE:
		case CH_MBC1_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: 'MBC1'", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MBC2_CODE:
		case CH_MBC2_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC2());
			memoryBankControllerType = MemoryBankControllerType::MBC2;
			Logger::WriteInfo("Cartridge type: 'MBC2'", CARTRIDGE_LOG_HEADER);
			break;
		case CH_ROM_RAM_CODE:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: 'ROM + RAM'", CARTRIDGE_LOG_HEADER);
			break;
		case CH_ROM_RAM_BATTERY_CODE:
			break;
		case CH_MMM01_CODE:
		case CH_MMM01_RAM_CODE:
		case CH_MMM01_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MMM01());
			memoryBankControllerType = MemoryBankControllerType::MMM01;
			Logger::WriteInfo("Cartridge type: 'MMM01'", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MBC3_TIMER_BATTERY_CODE:
		case CH_MBC3_TIMER_RAM_BATTERY_CODE:
		case CH_MBC3_CODE:
		case CH_MBC3_RAM_CODE:
		case CH_MBC3_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC3());
			memoryBankControllerType = MemoryBankControllerType::MBC3;
			Logger::WriteInfo("Cartridge type: 'MBC3'", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MBC5_CODE:
		case CH_MBC5_RAM_CODE:
		case CH_MBC5_RAM_BATTERY_CODE:
		case CH_MBC5_RUMBLE_CODE:
		case CH_MBC5_RUMBLE_RAM_CODE:
		case CH_MBC5_RUMBLE_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC5());
			memoryBankControllerType = MemoryBankControllerType::MBC5;
			Logger::WriteInfo("Cartridge type: MBC5", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MBC6_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC6());
			memoryBankControllerType = MemoryBankControllerType::MBC6;
			Logger::WriteInfo("Cartridge type: MBC6", CARTRIDGE_LOG_HEADER);
			break;
		case CH_MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC7());
			memoryBankControllerType = MemoryBankControllerType::MBC7;
			Logger::WriteInfo("Cartridge type: MBC7", CARTRIDGE_LOG_HEADER);
			break;
		case 0xFC:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: POCKET CAMERA", CARTRIDGE_LOG_HEADER);
			break;
		case CH_HuC3_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new HuC3());
			memoryBankControllerType = MemoryBankControllerType::HuC3;
			Logger::WriteInfo("Cartridge type: HuC3", CARTRIDGE_LOG_HEADER);
			break;
		case CH_HuC1_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new HuC1());
			memoryBankControllerType = MemoryBankControllerType::HuC1;
			Logger::WriteInfo("Cartridge type: HuC1", CARTRIDGE_LOG_HEADER);
			break;
		}
	}

	void Cartridge::DecodeROMSize(uint8_t byte)
	{
		// Calculation reference: https://gbdev.io/pandocs/#the-cartridge-header
		 uint64_t romSize = (32 << byte) * KiB;

		// Display the ROM size (in KiB) for debugging purposes
		uint16_t romSizeKB = romSize / (float)KiB;
		Logger::WriteInfo("Cartridge ROM available:  " + std::to_string(romSizeKB) + " KiB", CARTRIDGE_LOG_HEADER);
	}

	void Cartridge::DecodeRAMSize(uint8_t byte)
	{
		uint32_t ramSize = 0;

		switch (byte)
		{
		case CH_NO_RAM:
		case CH_UNUSED_RAM:
			// No RAM
			break;
		case CH_8KB_RAM:
			ramSize = 8 * KiB;
			break;
		case CH_32KB_RAM:
			ramSize = 32 * KiB;
			break;
		case CH_128KB_RAM:
			ramSize = 128 * KiB;
			break;
		case CH_64KB_RAM:
			ramSize = 64 * KiB;
			break;
		}

		ram = std::vector<uint8_t>(ramSize);

		// Display the RAM size (in KiB) for debugging purposes
		uint16_t ramSizeKB = ramSize / (float)KiB;
		Logger::WriteInfo("Cartridge RAM available: " + std::to_string(ramSizeKB) + " KiB", CARTRIDGE_LOG_HEADER);
	}

	MemoryBankControllerType Cartridge::GetMemoryBankControllerType()
	{
		return memoryBankControllerType;
	}

	uint8_t Cartridge::Read(uint16_t address)
	{
		switch (memoryBankControllerType)
		{
		case MemoryBankControllerType::None:
			// TODO: The address may also refer to RAM

			if (address > rom.size())
			{
				Logger::WriteError("Attempted to read from invalid cartridge address: " + GetHexString16(address), CARTRIDGE_LOG_HEADER);
				return 0;
			}

			return rom[address];
		default:
			// If the memoryBankControllerType is anything other than None, then the memoryBankController should not be NULL
			assert(memoryBankController != nullptr);
			return memoryBankController->Read(address);
		}
	}

	void Cartridge::Write(uint16_t address, uint8_t value)
	{
		switch (memoryBankControllerType)
		{
		case MemoryBankControllerType::None:
			// TODO: The address may also refer to RAM

			if (address > rom.size())
			{
				Logger::WriteWarning("Attempted to write to invalid cartridge address: " + GetHexString16(address), CARTRIDGE_LOG_HEADER);
				return;
			}

			rom[address] = value;
			break;
		default:
			// If the memoryBankControllerType is anything other than None, then the memoryBankController should not be NULL
			assert(memoryBankController != NULL);
			memoryBankController->Write(address, value);
			break;
		}
	}

	uint32_t Cartridge::GetROMSize()
	{
		return rom.size();
	}

	uint32_t Cartridge::GetRAMSize()
	{
		return ram.size();
	}

	bool Cartridge::IsAddressAvailable(uint16_t address)
	{
		// TODO: Account for RAM in cartridges without memory banks.
		return (memoryBankController != nullptr && memoryBankController->IsAddressAvailable(address)) ||
			(memoryBankControllerType == MemoryBankControllerType::None && rom.size() > address);
	}

	bool Cartridge::IsROMLoaded()
	{
		return isROMLoaded;
	}
}