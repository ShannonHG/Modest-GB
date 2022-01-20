#pragma once
#include <array>
#include "Memory/MemoryBankController.hpp"
#include "Memory/Register8.hpp"

namespace ModestGB
{
	class MBC5 : public MemoryBankController
	{
	public:
		MBC5();

		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		void Reset() override;

	protected:
		std::string GetName() const override;

	private:
		bool isRamEnabled = false;
		uint16_t romBankNumber = 0;
		uint8_t ramBankNumber = 0;
	};
}