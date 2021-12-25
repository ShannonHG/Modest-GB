#pragma once
#include <array>
#include "Memory/MemoryBankController.hpp"
#include "Memory/Register8.hpp"

namespace SHG
{
	class MBC1 : public MemoryBankController
	{
	public:
		MBC1();
		
		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;

	protected:
		std::string GetName() const override;

	private:
		bool isSimpleBankingModeEnabled = true;
		bool isRamEnabled = false;
		uint8_t romBankNumber = 1;
		uint8_t ramBankNumber = 0;

		uint8_t GetAdjustedROMBankNumber(uint8_t romBankNumber) const;
	};
}