#pragma once
#include <array>
#include <map>
#include "Memory/MemoryBankController.hpp"
#include "Memory/Register8.hpp"

namespace ModestGB
{
	class MBC3 : public MemoryBankController
	{
	public:
		MBC3();

		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		void Reset() override;

	protected:
		std::string GetName() const override;

	private:
		// Enables RAM and RTC registers
		bool isRAMAndRTCEnabled = false;
		bool isSimpleRAMBankingMode = false;
		bool isLatchingReady = false;
		uint8_t romBankNumber = 1;
		uint8_t ramBankNumber = 0;
		std::map<uint8_t, uint8_t> rtcRegisters;
	};
}