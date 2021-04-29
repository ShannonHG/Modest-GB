#pragma once
#include <string>
#include <map>
#include <vector>
#include "Globals.hpp"
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

	// Catridge header memory bank controller type codes
	const uint8_t CH_ROM_ONLY_CODE = 0x00;
	const uint8_t CH_MBC1_CODE = 0x01;
	const uint8_t CH_MBC1_RAM_CODE = 0x02;
	const uint8_t CH_MBC1_RAM_BATTERY_CODE = 0x03;
	const uint8_t CH_MBC2_CODE = 0x05;
	const uint8_t CH_MBC2_BATTERY_CODE = 0x06;
	const uint8_t CH_ROM_RAM_CODE = 0x08;
	const uint8_t CH_ROM_RAM_BATTERY_CODE = 0x09;
	const uint8_t CH_MMM01_CODE = 0x0B; 
	const uint8_t CH_MMM01_RAM_CODE = 0x0C;
	const uint8_t CH_MMM01_RAM_BATTERY_CODE = 0x0D;
	const uint8_t CH_MBC3_TIMER_BATTERY_CODE = 0x0F;
	const uint8_t CH_MBC3_TIMER_RAM_BATTERY_CODE = 0x10;
	const uint8_t CH_MBC3_CODE = 0x11;
	const uint8_t CH_MBC3_RAM_CODE = 0x12;
	const uint8_t CH_MBC3_RAM_BATTERY_CODE = 0x13;
	const uint8_t CH_MBC5_CODE = 0x19;
	const uint8_t CH_MBC5_RAM_CODE = 0x1A;
	const uint8_t CH_MBC5_RAM_BATTERY_CODE = 0x1B;
	const uint8_t CH_MBC5_RUMBLE_CODE = 0x1C;
	const uint8_t CH_MBC5_RUMBLE_RAM_CODE = 0x1D;
	const uint8_t CH_MBC5_RUMBLE_RAM_BATTERY_CODE = 0x1E;
	const uint8_t CH_MBC6_CODE = 0x20;
	const uint8_t CH_MBC7_SENSOR_RUMBLE_RAM_BATTERY_CODE = 0x22;
	const uint8_t CH_HuC3_CODE = 0xFE;
	const uint8_t CH_HuC1_RAM_BATTERY_CODE = 0xFF;

	// Catridge header RAM type codes
	const uint8_t CH_NO_RAM = 0x00; // No RAM
	const uint8_t CH_UNUSED_RAM = 0x01; // Unused
	const uint8_t CH_8KB_RAM = 0x02; // 1 bank
	const uint8_t CH_32KB_RAM = 0x03; // 4 banks of 8 KiB each 
	const uint8_t CH_128KB_RAM = 0x04; // 16 banks of 8 KiB each 
	const uint8_t CH_64KB_RAM = 0x05; // 8 banks of 8 KiB each

	// Catridge header ROM type codes
	const uint8_t CH_32KB_ROM = 0x00; // 2 banks of 16 KiB each
	const uint8_t CH_64KB_ROM = 0x01; // 4 banks of 16 KiB each
	const uint8_t CH_128KB_ROM = 0x02; // 8 banks of 16 KiB each
	const uint8_t CH_256KB_ROM = 0x03; // 16 banks of 16 KiB each
	const uint8_t CH_512KB_ROM = 0x04; // 32 banks of 16 KiB each
	const uint8_t CH_1MB_ROM = 0x05; // 64 banks of 16 KiB each
	const uint8_t CH_2MB_ROM = 0x06; // 128 banks of 16 KiB each
	const uint8_t CH_4MB_ROM = 0x07; // 256 banks of 16 KiB each
	const uint8_t CH_8MB_ROM = 0x08; // 512 banks of 16 KiB each

	// Catridge header addresses
	static const uint16_t CH_CGB_FLAG_ADDRESS = 0x0143;
	static const uint16_t CH_MEMORY_BANK_TYPE_ADDRESS = 0x0147;
	static const uint16_t CH_ROM_SIZE_ADDRESS = 0x0148;
	static const uint16_t CH_RAM_SIZE_ADDRESS = 0x0149;

	class Cartridge
	{
	public:
		/// <summary>
		/// Loads a ROM from a file
		/// </summary>
		/// <param name="romFilePath"></param>
		/// <returns></returns>
		bool LoadFromFile(std::string romFilePath);

		/// <summary>
		/// Loads a ROM from in-memory data
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		bool LoadFromMemory(std::vector<uint8_t>& data);

		/// <summary>
		/// The type of memory bank controller this catridge uses.
		/// </summary>
		/// <returns></returns>
		MemoryBankControllerType GetMemoryBankControllerType();

		/// <summary>
		/// Size of the ROM included in this cartridge (if any)
		/// </summary>
		/// <returns></returns>
		uint32_t GetROMSize();

		/// <summary>
		/// Size of the RAM included in this cartridge (if any)
		/// </summary>
		/// <returns></returns>
		uint32_t GetRAMSize();

	private:
		std::unique_ptr<MemoryBankController> memoryBankController;
		MemoryBankControllerType memoryBankControllerType;

		std::vector<uint8_t> ram;
		std::vector<uint8_t> rom;

		void DecodeCartridgeType(uint8_t byte);
		void DecodeROMSize(uint8_t byte);
		void DecodeRAMSize(uint8_t byte);
		void DecodeROMByte(uint8_t byte, uint16_t address);

		bool TryGetByte(uint16_t address, uint8_t& outValue);
		bool TrySetByte(uint16_t address, uint8_t value);
	};
}