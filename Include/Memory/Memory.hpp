#pragma once
#include <vector>
#include "Memory/DataStorageDevice.hpp"

namespace SHG
{
	class Memory : public DataStorageDevice
	{
	public:
		Memory(uint32_t memorySize);
		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;
	private:
		std::vector<uint8_t> memory;
	};
}