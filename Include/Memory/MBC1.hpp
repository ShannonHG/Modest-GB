#pragma once
#include <array>
#include "Memory/MemoryBankController.hpp"

namespace SHG
{
	class MBC1 : public MemoryBankController
	{
	public:
		bool TryGetByte(uint16_t address, uint8_t& outValue) override;
		bool TrySetByte(uint16_t address, uint8_t value) override;
		uint8_t GetRAMBankNumber();
		uint8_t GetROMBankNumber();
		uint8_t GetBankingMode();
		bool IsRAMEnabled();

	protected:
		std::string GetName() override;

	private:
		std::array<uint8_t, 4> registers{};
	};
}