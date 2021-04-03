#pragma once
#include "MemoryBankController.hpp"

namespace SHG
{
	class MBC5 : public MemoryBankController
	{
	public:
		uint8_t GetByte(uint16_t address) override;
		void SetByte(uint16_t address, uint8_t value) override;
	};
}