#pragma once
#include <string>

namespace SHG
{
	class Cartridge
	{
	public: 
		bool LoadFromFile(std::string romFilePath);

	private:
		static const uint16_t CGB_FLAG_INDEX = 0x0143;
		static const uint16_t CARTRIDGE_TYPE_INDEX = 0x0147;
		static const uint16_t ROM_SIZE_INDEX = 0x0148;
		static const uint16_t RAM_SIZE_INDEX = 0x0149;

		uint32_t romSize = 0;
		uint32_t ramSize = 0;

		void ParseCartridgeType(uint8_t byte);
		void ParseROMSize(uint8_t byte);
		void ParseRAMSize(uint8_t byte);
	};
}