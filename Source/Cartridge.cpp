#include <fstream>
#include <cmath>
#include "Cartridge.hpp"
#include "Logger.hpp"

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

			uint8_t currentByte = buffer[0];

			ParseHeaderByte(currentByte, address);

			address++;
		}

		return true;
	}

	bool Cartridge::LoadFromData(std::vector<uint8_t>& data)
	{
		uint16_t address = 0;
		for (uint8_t currentByte : data)
		{
			switch (address)
			{
			case CH_CGB_FLAG_ADDRESS:
				// If the most significant bit is enabled, then CGB mode is enabled.
				if ((currentByte & 0b10000000) == 0b10000000) Logger::Write("CGB mode is enabled");
				break;
			case CH_MEMORY_BANK_TYPE_ADDRESS:
				ParseCartridgeType(currentByte);
				break;
			case CH_ROM_SIZE_ADDRESS:
				ParseROMSize(currentByte);
				break;
			case CH_RAM_SIZE_ADDRESS:
				ParseRAMSize(currentByte);
				break;
			}

			address++;
		}

		return true;
	}

	void Cartridge::ParseHeaderByte(uint8_t byte, uint16_t address)
	{
		switch (address)
		{
		case CH_CGB_FLAG_ADDRESS:
			// If the most significant bit is enabled, then CGB mode is enabled.
			if ((byte & 0b10000000) == 0b10000000) Logger::Write("CGB mode is enabled");
			break;
		case CH_MEMORY_BANK_TYPE_ADDRESS:
			ParseCartridgeType(byte);
			break;
		case CH_ROM_SIZE_ADDRESS:
			ParseROMSize(byte);
			break;
		case CH_RAM_SIZE_ADDRESS:
			ParseRAMSize(byte);
			break;
		}
	}

	void Cartridge::ParseCartridgeType(uint8_t byte)
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
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::Write("Cartridge type is 'MBC1'");
			break;
		case CH_MBC2_CODE:
		case CH_MBC2_BATTERY_CODE:
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
			memoryBankControllerType = MemoryBankControllerType::MMM01;
			Logger::Write("Cartridge type is 'MMM01'");
			break;
		case CH_MBC3_TIMER_BATTERY_CODE:
		case CH_MBC3_TIMER_RAM_BATTERY_CODE:
		case CH_MBC3_CODE:
		case CH_MBC3_RAM_CODE:
		case CH_MBC3_RAM_BATTERY_CODE:
			memoryBankControllerType = MemoryBankControllerType::MBC3;
			Logger::Write("Cartridge type is 'MBC3'");
			break;
		case CH_MBC5_CODE:
		case CH_MBC5_RAM_CODE:
		case CH_MBC5_RAM_BATTERY_CODE:
		case CH_MBC5_RUMBLE_CODE:
		case CH_MBC5_RUMBLE_RAM_CODE:
		case CH_MBC5_RUMBLE_RAM_BATTERY_CODE:
			memoryBankControllerType = MemoryBankControllerType::MBC5;
			Logger::Write("Cartridge type is 'MBC5'");
			break;
		case CH_MBC6_CODE:
			memoryBankControllerType = MemoryBankControllerType::MBC6;
			Logger::Write("Cartridge type is 'MBC6'");
			break;
		case CH_MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE:
			memoryBankControllerType = MemoryBankControllerType::MBC7;
			Logger::Write("Cartridge type is 'MBC7'");
			break;
		case 0xFC:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::Write("Cartridge type is 'POCKET CAMERA'");
			break;
		case CH_HuC3_CODE:
			memoryBankControllerType = MemoryBankControllerType::HuC3;
			Logger::Write("Cartridge type is 'HuC3'");
			break;
		case CH_HuC1_RAM_BATTERY_CODE:
			memoryBankControllerType = MemoryBankControllerType::HuC1;
			Logger::Write("Cartridge type is 'HuC1'");
			break;
		}
	}

	void Cartridge::ParseROMSize(uint8_t byte)
	{
		switch (byte)
		{
		default:
			// Calculation reference: https://gbdev.io/pandocs/#the-cartridge-header
			romSize = (32 << byte) * KiB; 
			break;
		}

		double romSizeMB = romSize / MiB;
		Logger::Write("Cartridge ROM available:  " + std::to_string(romSizeMB) + " MiB");
	}

	void Cartridge::ParseRAMSize(uint8_t byte)
	{
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

		double ramSizeMB = ramSize / MiB;
		Logger::Write("Cartridge RAM available: " + std::to_string(ramSizeMB) + " MiB");
	}

	MemoryBankControllerType Cartridge::GetMemoryBankControllerType()
	{
		return memoryBankControllerType;
	}
	
	uint32_t Cartridge::GetROMSize()
	{
		return romSize;
	}

	uint32_t Cartridge::GetRAMSize()
	{
		return ramSize;
	}
}