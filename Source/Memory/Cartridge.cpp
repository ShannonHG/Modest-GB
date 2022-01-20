#include <cmath>
#include <cassert>
#include <iterator>
#include <filesystem>
#include "Logger.hpp"
#include "Memory/Cartridge.hpp"
#include "Logger.hpp"
#include "Memory/MBC1.hpp"
#include "Memory/MBC3.hpp"
#include "Memory/MBC5.hpp"

namespace ModestGB
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

	const uint16_t LOGO_START_ADDRESS = 0x104;
	const uint16_t LOGO_END_ADDRESS = 0x133;

	const uint16_t HEADER_SIZE_IN_BYTES = 0x143;

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

	const std::array<uint8_t, 48> LOGO_BITMAP =
	{
		 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};

	bool Cartridge::Load(const std::string& romFilePath)
	{
		std::string relativeSavedDataPath = std::filesystem::path(romFilePath).filename().replace_extension(".sav").string();

		switch (savedDataSearchType)
		{
		case SavedDataSearchType::ROM_DIRECTORY:
			// Append the relative saved data path to the ROM directory path.
			savedDataPath = (std::filesystem::path(romFilePath).parent_path() / relativeSavedDataPath).string();
			break;
		case SavedDataSearchType::EMULATOR_DIRECTORY:
		{
			std::filesystem::path savedDataDir = std::filesystem::current_path() / "Saved-Data";

			if (!std::filesystem::exists(savedDataDir))
			{
				Logger::WriteInfo("Creating saved directory: " + savedDataDir.string());
				std::filesystem::create_directory(savedDataDir);
			}

			// Append the relative saved data path to the saved data dir.
			savedDataPath = (savedDataDir / relativeSavedDataPath).string();
			break;
		}
		}

		auto file = std::ifstream(romFilePath, std::ios::binary);

		if (!file.is_open())
			return false;

		// Load the ROM file into a vector of bytes.
		auto romData = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		return Load(romData);
	}

	bool Cartridge::Load(const std::vector<uint8_t>& romData)
	{
		if (romData.size() < HEADER_SIZE_IN_BYTES)
			return false;

		romTitle.clear();
		for (uint32_t address = 0; address < romData.size(); address++)
		{
			if (address >= TITLE_START_ADDRESS && address <= TITLE_END_ADDRESS)
			{
				romTitle += (char)romData[address];
			}
			else if (address >= LOGO_START_ADDRESS && address <= LOGO_END_ADDRESS)
			{
				// If the ROM does not contain the correct bitmap data for the logo,
				// then the ROM should be considered invalid.
				if (LOGO_BITMAP.at(address - LOGO_START_ADDRESS) != romData[address])
					return false;
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
		Logger::WriteInfo("ROM Title: " + std::string(romTitle.c_str()), CARTRIDGE_LOG_HEADER);

		// Attach ROM and RAM to the memory bank controller.
		if (memoryBankController != nullptr)
		{
			if (GetRAMSize() > 0)
			{
				Logger::WriteInfo("Attaching RAM to MBC", CARTRIDGE_LOG_HEADER);
				memoryBankController->AttachRAM(ram);
				memoryBankController->SetRAMWriteCallback(std::bind(&Cartridge::OnRAMWrite, this, std::placeholders::_1, std::placeholders::_2));

				// Load saved RAM data (if any), from the saved data path.
				OpenSavedDataFile();
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
		romTitle.clear();
	}

	const std::string& Cartridge::GetROMTitle() const
	{
		return romTitle;
	}

	void Cartridge::SetSavedDataSearchType(SavedDataSearchType searchType)
	{
		savedDataSearchType = searchType;
	}

	SavedDataSearchType Cartridge::GetSavedDataSearchType() const
	{
		return savedDataSearchType;
	}

	bool Cartridge::IsROMLoaded()
	{
		return isROMLoaded;
	}

	uint8_t Cartridge::Read(uint16_t address) const
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
				uint16_t normalizedAddress = address - OPTIONAL_8KB_RAM_START_ADDRESS;
				ram[normalizedAddress] = value;
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
		return static_cast<uint32_t>(rom.size());
	}

	uint32_t Cartridge::GetRAMSize()
	{
		return static_cast<uint32_t>(ram.size());
	}

	MemoryBankControllerType Cartridge::GetMemoryBankControllerType()
	{
		return memoryBankControllerType;
	}

	void Cartridge::DecodeROMSize(uint8_t byte)
	{
		// Calculation reference: https://gbdev.io/pandocs/#the-cartridge-header
		uint32_t romSize = (32 << byte) * KiB;

		// Display the ROM size (in KiB) for debugging purposes
		uint16_t romSizeKB = static_cast<uint16_t>(romSize / (float)KiB);
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
		uint16_t ramSizeKB = static_cast<uint16_t>(ramSize / (float)KiB);
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
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: 'MBC2'", CARTRIDGE_LOG_HEADER);
			WriteUnsupportedMBCMesage();
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
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: 'MMM01'", CARTRIDGE_LOG_HEADER);
			WriteUnsupportedMBCMesage();
			break;
		case MBC3_TIMER_BATTERY_CODE:
		case MBC3_TIMER_RAM_BATTERY_CODE:
		case MBC3_CODE:
		case MBC3_RAM_CODE:
		case MBC3_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC3());
			memoryBankControllerType = MemoryBankControllerType::MBC3;
			Logger::WriteInfo("Cartridge type: 'MBC3'", CARTRIDGE_LOG_HEADER);
			break;
		case MBC5_CODE:
		case MBC5_RAM_CODE:
		case MBC5_RAM_BATTERY_CODE:
		case MBC5_RUMBLE_CODE:
		case MBC5_RUMBLE_RAM_CODE:
		case MBC5_RUMBLE_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC5());
			memoryBankControllerType = MemoryBankControllerType::MBC5;
			Logger::WriteInfo("Cartridge type: MBC5", CARTRIDGE_LOG_HEADER);
			break;
		case MBC6_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: MBC6", CARTRIDGE_LOG_HEADER);
			WriteUnsupportedMBCMesage();
			break;
		case MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: MBC7", CARTRIDGE_LOG_HEADER);
			WriteUnsupportedMBCMesage();
			break;
		case 0xFC:
			memoryBankControllerType = MemoryBankControllerType::None;
			Logger::WriteInfo("Cartridge type: POCKET CAMERA", CARTRIDGE_LOG_HEADER);
			break;
		case HuC3_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: HuC3", CARTRIDGE_LOG_HEADER);
			WriteUnsupportedMBCMesage();
			break;
		case HuC1_RAM_BATTERY_CODE:
			memoryBankController = std::unique_ptr<MemoryBankController>(new MBC1());
			memoryBankControllerType = MemoryBankControllerType::MBC1;
			Logger::WriteInfo("Cartridge type: HuC1", CARTRIDGE_LOG_HEADER);
			WriteUnsupportedMBCMesage();
			break;
		}
	}

	void Cartridge::OpenSavedDataFile()
	{
		if (savedDataStream.is_open())
			savedDataStream.close();

		// Attempt to open an existing saved data file. Since std::ios::in is used, 
		// the stream will not attempt to create the file.
		savedDataStream.open(savedDataPath, std::ios::in | std::ios::out | std::ofstream::binary);

		// If there was no existing saved data file, then create one.
		if (!savedDataStream.is_open())
			savedDataStream.open(savedDataPath, std::ios::out | std::ofstream::binary);

		// If the saved data file is still not open, then a problem ocurred.
		if (!savedDataStream.is_open())
		{
			Logger::WriteError("Failed to open saved data file: " + savedDataPath, CARTRIDGE_LOG_HEADER);
			return;
		}

		Logger::WriteInfo("Saved data will be stored at: " + savedDataPath, CARTRIDGE_LOG_HEADER);

		int count = 0;
		while (!savedDataStream.eof() && count < ram.size())
		{
			char byteBuffer[1];
			savedDataStream.read(byteBuffer, 1);
			ram[count] = static_cast<uint8_t>(byteBuffer[0]);

			count++;
		}

		savedDataStream.clear();
	}

	void Cartridge::OnRAMWrite(uint16_t address, uint8_t value)
	{
		savedDataStream.seekp(address);

		char outByte = static_cast<char>(value);
		savedDataStream.write(&outByte, 1);
	}

	bool Cartridge::IsRAMAddress(uint16_t address) const
	{
		return address >= OPTIONAL_8KB_RAM_START_ADDRESS && address <= OPTIONAL_8KB_RAM_END_ADDRESS;
	}

	void Cartridge::WriteUnsupportedMBCMesage()
	{
		Logger::WriteError("The requested memory bank controller type is not supported, MBC1 will be used instead.", CARTRIDGE_LOG_HEADER);
	}
}