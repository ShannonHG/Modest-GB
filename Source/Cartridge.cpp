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

		int index = 0;

		while (!file.eof())
		{
			file.read(buffer, 1);

			uint8_t currentByte = buffer[0];

			switch (index)
			{
			case CGB_FLAG_INDEX:
				// If the most significant bit is enabled, then CGB mode is enabled.
				if ((currentByte & 0b10000000) == 0b10000000) Logger::Write("CGB mode is enabled");
				break;
			case CARTRIDGE_TYPE_INDEX:
				ParseCartridgeType(currentByte);
				break;
			case ROM_SIZE_INDEX:
				ParseROMSize(currentByte);
				break;
			case RAM_SIZE_INDEX:
				ParseRAMSize(currentByte);
				break;
			}

			index++;
		}

		return true;
	}

	void Cartridge::ParseCartridgeType(uint8_t byte)
	{
		switch (byte)
		{
		case 0x00:
			Logger::Write("Cartridge type is 'ROM ONLY'");
			break;
		case 0x01:
		case 0x02:
		case 0x03:
			Logger::Write("Cartridge type is 'MBC1'");
			break;
		case 0x05:
		case 0x06:
			Logger::Write("Cartridge type is 'MBC2'");
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0B:
		case 0x0C:
		case 0x0D:
			Logger::Write("Cartridge type is 'MMM01'");
			break;
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			Logger::Write("Cartridge type is 'MBC3'");
			break;
		case 0x19:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
			Logger::Write("Cartridge type is 'MBC5'");
			break;
		case 0x20:
			Logger::Write("Cartridge type is 'MBC6'");
			break;
		case 0x22:
			Logger::Write("Cartridge type is 'MBC7'");
			break;
		case 0xFC:
			Logger::Write("Cartridge type is 'POCKET CAMERA'");
			break;
		case 0xFD:
			Logger::Write("Cartridge type is 'BANDAI TAMA5'");
			break;
		case 0xFE:
			Logger::Write("Cartridge type is 'HuC3'");
			break;
		case 0xFF:
			Logger::Write("Cartridge type is 'HuC1'");
			break;
		}
	}

	void Cartridge::ParseROMSize(uint8_t byte)
	{
		switch (byte)
		{
		default:
			romSize = ((32 << byte) + 1) * 1024;
			break;
		case 0x52:
			romSize = 1100000;
			break;
		case 0x53:
			romSize = 1200000;
			break;
		case 0x54:
			romSize = 1500000;
			break;
		}

		double romSizeMB = romSize / 1000000;
		Logger::Write("Cartridge ROM available:  " + std::to_string(romSizeMB) + " MB");
	}

	void Cartridge::ParseRAMSize(uint8_t byte)
	{
		switch (byte)
		{
		case 0x00:
		case 0x01:
			// No RAM
			break;
		case 0x02:
			ramSize = 8000;
			break;
		case 0x03:
			ramSize = 32000;
			break;
		case 0x04:
			ramSize = 128000;
			break;
		case 0x05:
			ramSize = 64000;
			break;
		}

		double ramSizeMB = ramSize / 1000000;
		Logger::Write("Cartridge RAM available: " + std::to_string(ramSizeMB) + " MB");
	}
}