#pragma once
#include <vector>
#include "Memory/Memory.hpp"

namespace SHG
{
	class BasicMemory : public Memory
	{
	public:
		BasicMemory(uint32_t size);
		bool IsAddressAvailable(uint16_t address) const;

		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		void Reset() override;
	private:
		std::vector<uint8_t> data;
	};
}