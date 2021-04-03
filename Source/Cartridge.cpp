#include <fstream>
#include <cmath>
#include "Cartridge.hpp"
#include "Logger.hpp"
#include "MBC1.hpp"
#include "MBC2.hpp"
#include "MBC3.hpp"
#include "MBC5.hpp"
#include "MBC6.hpp"
#include "MBC7.hpp"
#include "HuC1.hpp"
#include "HuC3.hpp"
#include "MMM01.hpp"

namespace SHG
{
	bool Cartridge::LoadFromFile(std::string romFilePath)
	{
		std::ifstream file(romFilePath, std::ios::binary);

		if (!file.is_open())
		{
			Logger::WriteError("Invalid ROM file provided");
			return false;
		}

		// Buffer for storing the current byte
		char buffer[1];

		int address = 0;

		while (!file.eof())
		{
			file.read(buffer, 1);

			DecodeROMByte(buffer[0], address);

			address++;
		}

		return true;
	}

	bool Cartridge::LoadFromData(std::vector<uint8_t>& data)
	{
		uint16_t address = 0;
		for (uint8_t byte : data)
		{
			DecodeROMByte(byte, address);

			address++;
		}

		return true;
	}

	void Cartridge::DecodeROMByte(uint8_t byte, uint16_t address)
	{
		switch (address)
		{
		case CH_CGB_FLAG_ADDRESS:
			// If the most significant bit is enabled, then CGB mode is enabled.
			if ((byte & 0b10000000) == 0b10000000) Logger::Write("CGB mode is enabled");
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
			Logger::Write("Cartridge type is 'ROM ONLY'");
			break;
		case CH_MBC1_CODE:
		case CH_MBC1_RAM_CODE:
		case CH_MBC1_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::Write("Cartridge type is 'MBC1'");
			break;
		case CH_MBC2_CODE:
		case CH_MBC2_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC2());
			memoryBankControllerType = MemoryBankControllerType::MBC2;
			Logger::Write("Cartridge type is 'MBC2'");
			break;
		case CH_ROM_RAM_CODE:
			break;
		case CH_ROM_RAM_BATTERY_CODE:
			break;
		case CH_MMM01_CODE:
		case CH_MMM01_RAM_CODE:
		case CH_MMM01_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MMM01());
			memoryBankControllerType = MemoryBankControllerType::MMM01;
			Logger::Write("Cartridge type is 'MMM01'");
			break;
		case CH_MBC3_TIMER_BATTERY_CODE:
		case CH_MBC3_TIMER_RAM_BATTERY_CODE:
		case CH_MBC3_CODE:
		case CH_MBC3_RAM_CODE:
		case CH_MBC3_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC3());
			memoryBankControllerType = MemoryBankControllerType::MBC3;
			Logger::Write("Cartridge type is 'MBC3'");
			break;
		case CH_MBC5_CODE:
		case CH_MBC5_RAM_CODE:
		case CH_MBC5_RAM_BATTERY_CODE:
		case CH_MBC5_RUMBLE_CODE:
		case CH_MBC5_RUMBLE_RAM_CODE:
		case CH_MBC5_RUMBLE_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC5());
			memoryBankControllerType = MemoryBankControllerType::MBC5;
			Logger::Write("Cartridge type is 'MBC5'");
			break;
		case CH_MBC6_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC6());
			memoryBankControllerType = MemoryBankControllerType::MBC6;
			Logger::Write("Cartridge type is 'MBC6'");
			break;
		case CH_MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC7());
			memoryBankControllerType = MemoryBankControllerType::MBC7;
			Logger::Write("Cartridge type is 'MBC7'");
			break;
		case 0xFC:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::Write("Cartridge type is 'POCKET CAMERA'");
			break;
		case CH_HuC3_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new HuC3());
			memoryBankControllerType = MemoryBankControllerType::HuC3;
			Logger::Write("Cartridge type is 'HuC3'");
			break;
		case CH_HuC1_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new HuC1());
			memoryBankControllerType = MemoryBankControllerType::HuC1;
			Logger::Write("Cartridge type is 'HuC1'");
			break;
		}
	}

	void Cartridge::DecodeROMSize(uint8_t byte)
	{
		uint64_t romSize = 0;

		switch (byte)
		{
		default:
			// Calculation reference: https://gbdev.io/pandocs/#the-cartridge-header
			romSize = (32 << byte) * KiB; 
			break;
		}

		double romSizeMB = romSize / (double)KiB;
		Logger::Write("Cartridge ROM available:  " + std::to_string(romSizeMB) + " KiB");
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

		double ramSizeMB = ramSize / (double)KiB;
		Logger::Write("Cartridge RAM available: " + std::to_string(ramSizeMB) + " KiB");
	}

	MemoryBankControllerType Cartridge::GetMemoryBankControllerType()
	{
		return memoryBankControllerType;
	}

	MemoryBankController* Cartridge::GetMemoryBankController()
	{
		return memoryBankController.get();
	}

	uint8_t Cartridge::GetByte(uint16_t address)
	{
		return 0;
	}

	void Cartridge::SetByte(uint16_t address, uint8_t value)
	{
	
	}
	
	uint32_t Cartridge::GetROMSize()
	{
		return rom.size();
	}

	uint32_t Cartridge::GetRAMSize()
	{
		return ram.size();
	}
}