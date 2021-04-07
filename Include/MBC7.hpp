#pragma once
#include "MemoryBankController.hpp"

namespace SHG
{
	class MBC7 : public MemoryBankController
	{
	public:
		bool TryGetByte(uint16_t address, uint8_t& outValue) override;
		bool TrySetByte(uint16_t address, uint8_t value) override;

	protected:
		std::string GetName() override;
	};
}
