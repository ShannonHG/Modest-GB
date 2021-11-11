#include <fstream>
#include <cmath>
#include <cassert>
#include <iterator>
#include "Logger.hpp"
#include "Memory/Cartridge.hpp"
#include "Logger.hpp"
#include "Memory/MBC1.hpp"

namespace SHG
{
	const std::string CARTRIDGE_LOG_HEADER = "[CART]";

	// Title - The title of the ame in upper case ASCII.
	const uint16_t TITLE_START_ADDRESS = 0x0134;
	const uint16_t TITLE_END_ADDRESS = 0x0143;

	// CGB Flag - The upper bit of this byte is uused to enable Game Boy Color functions. 
	// In older cartridges, this is a part of the title data.
	const uint16_t CGB_FLAG_ADDRESS = 0x0143;

	// SGB Flag - Specifies whether the game supports Super Game Boy functions. 
	const uint16_t SGB_FLAG_ADDRESS = 0x0146;

	// Cartridge Type - Indicates which Memory Bank Controller (if any) is used, and if other external hardware exists.
	const uint16_t CARTRIDGE_TYPE_ADDRESS = 0x0147;

	// ROM Size - Size of the ROM. Generally calculated as "32 KiB << ROM_SIZE_CODE".
	const uint16_t ROM_SIZE_ADDRESS = 0x0148;

	// RAM Size - Size of external RAM (if any).
	const uint16_t RAM_SIZE_ADDRESS = 0x0149;

	// Catridge header memory bank controller type codes

	const uint8_t ROM_ONLY_CODE = 0x00;
	const uint8_t MBC1_CODE = 0x01;
	const uint8_t MBC1_RAM_CODE = 0x02;
	const uint8_t MBC1_RAM_BATTERY_CODE = 0x03;
	const uint8_t MBC2_CODE = 0x05;
	const uint8_t MBC2_BATTERY_CODE = 0x06;
	const uint8_t ROM_RAM_CODE = 0x08;
	const uint8_t ROM_RAM_BATTERY_CODE = 0x09;
	const uint8_t MMM01_CODE = 0x0B;
	const uint8_t MMM01_RAM_CODE = 0x0C;
	const uint8_t MMM01_RAM_BATTERY_CODE = 0x0D;
	const uint8_t MBC3_TIMER_BATTERY_CODE = 0x0F;
	const uint8_t MBC3_TIMER_RAM_BATTERY_CODE = 0x10;
	const uint8_t MBC3_CODE = 0x11;
	const uint8_t MBC3_RAM_CODE = 0x12;
	const uint8_t MBC3_RAM_BATTERY_CODE = 0x13;
	const uint8_t MBC5_CODE = 0x19;
	const uint8_t MBC5_RAM_CODE = 0x1A;
	const uint8_t MBC5_RAM_BATTERY_CODE = 0x1B;
	const uint8_t MBC5_RUMBLE_CODE = 0x1C;
	const uint8_t MBC5_RUMBLE_RAM_CODE = 0x1D;
	const uint8_t MBC5_RUMBLE_RAM_BATTERY_CODE = 0x1E;
	const uint8_t MBC6_CODE = 0x20;
	const uint8_t MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE = 0x22;
	const uint8_t HuC3_CODE = 0xFE;
	const uint8_t HuC1_RAM_BATTERY_CODE = 0xFF;

	// Catridge header RAM type codes

	const uint8_t NO_RAM_CODE = 0x00; // No RAM
	const uint8_t UNUSED_RAM_CODE = 0x01; // Unused
	const uint8_t RAM_8KB_CODE = 0x02; // 1 bank
	const uint8_t RAM_32KB_CODE = 0x03; // 4 banks of 8 KiB each 
	const uint8_t RAM_128KB_CODE = 0x04; // 16 banks of 8 KiB each 
	const uint8_t RAM_64KB_CODE = 0x05; // 8 banks of 8 KiB each

	// Catridge header ROM type codes

	const uint8_t ROM_32KB_CODE = 0x00; // 2 banks of 16 KiB each
	const uint8_t ROM_64KB_CODE = 0x01; // 4 banks of 16 KiB each
	const uint8_t ROM_128KB_CODE = 0x02; // 8 banks of 16 KiB each
	const uint8_t ROM_256KB_CODE = 0x03; // 16 banks of 16 KiB each
	const uint8_t ROM_512KB_CODE = 0x04; // 32 banks of 16 KiB each
	const uint8_t ROM_1MB_CODE = 0x05; // 64 banks of 16 KiB each
	const uint8_t ROM_2MB_CODE = 0x06; // 128 banks of 16 KiB each
	const uint8_t ROM_4MB_CODE = 0x07; // 256 banks of 16 KiB each
	const uint8_t ROM_8MB_CODE = 0x08; // 512 banks of 16 KiB each

	// Additional addresses
	const uint16_t OPTIONAL_8KB_RAM_START_ADDRESS = 0xA000;
	const uint16_t OPTIONAL_8KB_RAM_END_ADDRESS = 0xBFFF;

	bool Cartridge::Load(const std::string& romFilePath)
	{
		auto file = std::ifstream(romFilePath, std::ios::binary);

		if (!file.is_open())
			return false;

		// Load the ROM file into a vector of bytes.
		auto romData = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		return Load(romData);
	}

	bool Cartridge::Load(const std::vector<uint8_t>& romData)
	{
		title.clear();
		for (uint32_t address = 0; address < romData.size(); address++)
		{
			if (address >= TITLE_START_ADDRESS && address <= TITLE_END_ADDRESS)
			{
				title += (char)romData[address];
				// TODO: Account for CGB Flag
			}
			else if (address == CARTRIDGE_TYPE_ADDRESS)
			{
				InitializeMemoryBankController(romData[address]);
			}
			else if (address == ROM_SIZE_ADDRESS)
			{
				DecodeROMSize(romData[address]);
			}
			else if (address == RAM_SIZE_ADDRESS)
			{
				InitializeRAM(romData[address]);
			}
		}

		rom = romData;
		Logger::WriteInfo("ROM Title: " + std::string(title.c_str()));

		// Attach ROM and RAM to the memory bank controller.
		if (memoryBankController != nullptr)
		{
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
		title.clear();
	}

	bool Cartridge::IsROMLoaded()
	{
		return isROMLoaded;
	}

	uint8_t Cartridge::Read(uint16_t address)
	{
		switch (memoryBankControllerType)
		{
		case MemoryBankControllerType::None:
			if (ram.size() > 0 && IsRAMAddress(address))
			{
				// Normalize the address to the range [0, RAM_SIZE].
				return ram[address - OPTIONAL_8KB_RAM_START_ADDRESS];
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
			if (ram.size() > 0 && IsRAMAddress(address))
			{
				// Normalize the address to the range [0, RAM_SIZE].
				ram[address - OPTIONAL_8KB_RAM_START_ADDRESS] = value;
			}
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
		return
			// Does the address refer to a location in the memory bank controller?
			(memoryBankController != nullptr && memoryBankController->IsAddressAvailable(address)) ||
			// Does the address refer to a location in ROM?
			(memoryBankControllerType == MemoryBankControllerType::None && rom.size() > address) ||
			// Does the address refer to a location in RAM?
			(memoryBankControllerType == MemoryBankControllerType::None && ram.size() > 0 && IsRAMAddress(address));
	}

	MemoryBankControllerType Cartridge::GetMemoryBankControllerType()
	{
		return memoryBankControllerType;
	}

	void Cartridge::DecodeROMSize(uint8_t byte)
	{
		// Calculation reference: https://gbdev.io/pandocs/#the-cartridge-header
		 uint64_t romSize = (32 << byte) * KiB;

		// Display the ROM size (in KiB) for debugging purposes
		uint16_t romSizeKB = romSize / (float)KiB;
		Logger::WriteInfo("Cartridge ROM available:  " + std::to_string(romSizeKB) + " KiB", CARTRIDGE_LOG_HEADER);
	}

	void Cartridge::InitializeRAM(uint8_t byte)
	{
		uint32_t ramSize = 0;

		switch (byte)
		{
		case NO_RAM_CODE:
		case UNUSED_RAM_CODE:
			break;
		case RAM_8KB_CODE:
			ramSize = 8 * KiB;
			break;
		case RAM_32KB_CODE:
			ramSize = 32 * KiB;
			break;
		case RAM_128KB_CODE:
			ramSize = 128 * KiB;
			break;
		case RAM_64KB_CODE:
			ramSize = 64 * KiB;
			break;
		}

		ram = std::vector<uint8_t>(ramSize);

		// Display the RAM size (in KiB) for debugging purposes.
		uint16_t ramSizeKB = ramSize / (float)KiB;
		Logger::WriteInfo("Cartridge RAM available: " + std::to_string(ramSizeKB) + " KiB", CARTRIDGE_LOG_HEADER);
	}

	void Cartridge::InitializeMemoryBankController(uint8_t byte)
	{
		switch (byte)
		{
		case ROM_ONLY_CODE:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: 'ROM ONLY'", CARTRIDGE_LOG_HEADER);
			break;
		case MBC1_CODE:
		case MBC1_RAM_CODE:
		case MBC1_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: 'MBC1'", CARTRIDGE_LOG_HEADER);
			break;
		case MBC2_CODE:
		case MBC2_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC2());
			memoryBankControllerType = MemoryBankControllerType::MBC2;
			Logger::WriteInfo("Cartridge type: 'MBC2'", CARTRIDGE_LOG_HEADER);
			break;
		case ROM_RAM_CODE:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: 'ROM + RAM'", CARTRIDGE_LOG_HEADER);
			break;
		case ROM_RAM_BATTERY_CODE:
			break;
		case MMM01_CODE:
		case MMM01_RAM_CODE:
		case MMM01_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MMM01());
			memoryBankControllerType = MemoryBankControllerType::MMM01;
			Logger::WriteInfo("Cartridge type: 'MMM01'", CARTRIDGE_LOG_HEADER);
			break;
		case MBC3_TIMER_BATTERY_CODE:
		case MBC3_TIMER_RAM_BATTERY_CODE:
		case MBC3_CODE:
		case MBC3_RAM_CODE:
		case MBC3_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC3());
			memoryBankControllerType = MemoryBankControllerType::MBC3;
			Logger::WriteInfo("Cartridge type: 'MBC3'", CARTRIDGE_LOG_HEADER);
			break;
		case MBC5_CODE:
		case MBC5_RAM_CODE:
		case MBC5_RAM_BATTERY_CODE:
		case MBC5_RUMBLE_CODE:
		case MBC5_RUMBLE_RAM_CODE:
		case MBC5_RUMBLE_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC5());
			memoryBankControllerType = MemoryBankControllerType::MBC5;
			Logger::WriteInfo("Cartridge type: MBC5", CARTRIDGE_LOG_HEADER);
			break;
		case MBC6_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC6());
			memoryBankControllerType = MemoryBankControllerType::MBC6;
			Logger::WriteInfo("Cartridge type: MBC6", CARTRIDGE_LOG_HEADER);
			break;
		case MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new MBC7());
			memoryBankControllerType = MemoryBankControllerType::MBC7;
			Logger::WriteInfo("Cartridge type: MBC7", CARTRIDGE_LOG_HEADER);
			break;
		case 0xFC:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: POCKET CAMERA", CARTRIDGE_LOG_HEADER);
			break;
		case HuC3_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new HuC3());
			memoryBankControllerType = MemoryBankControllerType::HuC3;
			Logger::WriteInfo("Cartridge type: HuC3", CARTRIDGE_LOG_HEADER);
			break;
		case HuC1_RAM_BATTERY_CODE:
			//memoryBankController = std::unique_ptr<MemoryBankController>(new HuC1());
			memoryBankControllerType = MemoryBankControllerType::HuC1;
			Logger::WriteInfo("Cartridge type: HuC1", CARTRIDGE_LOG_HEADER);
			break;
		}
	}

	bool Cartridge::IsRAMAddress(uint16_t address)
	{
		return address >= OPTIONAL_8KB_RAM_START_ADDRESS && address <= OPTIONAL_8KB_RAM_END_ADDRESS;
	}
}