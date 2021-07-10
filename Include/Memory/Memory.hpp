#pragma once
#include <vector>
#include "Memory/DataStorageDevice.hpp"

namespace SHG
{
	class Memory : public DataStorageDevice
	{
	public:
		Memory(uint32_t memorySize);
		uint8_t GetByte(uint16_t address) override;
		void SetByte(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) override;

	private:
		std::vector<uint8_t> memory;
	};
}