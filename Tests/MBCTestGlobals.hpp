#pragma once
#include <vector>
#include <cstdint>

namespace SHG
{
	/// <summary>
	/// Generates ROM data with random values placed at specific addresses.
	/// </summary>
	/// <param name="size"> The size of the ROM in bytes </param>
	/// <param name="numOfBanks"> The number of ROM banks </param>
	/// <param name="offset"> The offset between random values. The first random value will always be placed at address 0x0000. </param>
	/// <returns></returns>
	std::vector<uint8_t> GenerateRandomTestROM(uint32_t size, uint32_t offset);

	/// <summary>
	/// Generates RAM data with random values placed at specific addresses.
	/// </summary>
	/// <param name="size"> The size of the RAM in bytes </param>
	/// <param name="numOfBanks"> The number of RAM banks </param>
	/// <param name="offset"> The offset between random values. The first random value will always be placed at address 0x0000. </param>
	/// <returns></returns>
	std::vector<uint8_t> GenerateRandomTestRAM(uint32_t size, uint32_t offset);

	/// <summary>
	/// Generates RAM data filled with zeroes
	/// </summary>
	/// <param name="size"> The size of the RAM in bytes </param>
	/// <returns></returns>
	std::vector<uint8_t> GenerateEmptyTestRAM(uint32_t size);

	/// <summary>
	/// Generates ROM data filled with zeroes
	/// </summary>
	/// <param name="size"> The size of the ROM in bytes </param>
	/// <returns></returns>
	std::vector<uint8_t> GenerateEmptyTestROM(uint32_t size);
}