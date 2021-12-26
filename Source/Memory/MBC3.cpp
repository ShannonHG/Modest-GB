#include <sstream>
#include <chrono>
#include "Memory/MBC3.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Arithmetic.hpp"
#include "Utils/SystemTime.hpp"

namespace SHG
{
	// Address range is read only
	const uint16_t ROM_BANK_X0_START_ADDR = 0x0000;
	const uint16_t ROM_BANK_X0_END_ADDR = 0x3FFF;

	// Address range is read only
	const uint16_t ROM_SWITCHABLE_BANK_START_ADDR = 0x4000;
	const uint16_t ROM_SWITCHABLE_BANK_END_ADDR = 0x7FFF;

	const uint16_t RAM_SWITCHABLE_BANK_START_ADDR = 0xA000;
	const uint16_t RAM_SWITCHABLE_BANK_END_ADDR = 0xBFFF;

	// Address range is write only
	const uint16_t RAM_AND_TIMER_ENABLE_START_ADDR = 0x0000;
	const uint16_t RAM_AND_TIMER_ENABLE_END_ADDR = 0x1FFF;

	// Address range is write only
	const uint16_t ROM_BANK_NUMBER_START_ADDR = 0x2000;
	const uint16_t ROM_BANK_NUMBER_END_ADDR = 0x3FFF;

	// Address range is write only
	const uint16_t RAM_BANK_NUMBER_RTC_SELECT_START_ADDR = 0x4000;
	const uint16_t RAM_BANK_NUMBER_RTC_SELECT_END_ADDR = 0x5FFF;

	// Address range is write only
	const uint16_t LATCH_CLOCK_DATA_START_ADDR = 0x6000;
	const uint16_t LATCH_CLOCK_DATA_END_ADDR = 0x7FFF;

	const uint16_t RTC_SECONDS_REGISTER_ADDR = 0x08;
	const uint16_t RTC_MINUTES_REGISTER_ADDR = 0x09;
	const uint16_t RTC_HOURS_REGISTER_ADDR = 0x0A;
	const uint16_t RTC_LOWER_DAY_COUNTER_REGISTER_ADDR = 0x0B;
	const uint16_t RTC_UPPER_DAY_COUNTER_REGISTER_ADDR = 0x0C;

	// TODO: Does not handle 1MB multi-game carts.
	MBC3::MBC3()
	{
		rtcRegisters =
		{
			{ 0x08, 0 },
			{ 0x09, 0 },
			{ 0x0A, 0 },
			{ 0x0B, 0 },
			{ 0x0C, 0 },
		};
	}

	std::string MBC3::GetName() const
	{
		return "MBC3";
	}

	uint8_t MBC3::Read(uint16_t address) const
	{
		if (address >= ROM_BANK_X0_START_ADDR && address <= ROM_BANK_X0_END_ADDR)
		{
			return ReadFromROM(address);
		}
		else if (address >= ROM_SWITCHABLE_BANK_START_ADDR && address <= ROM_SWITCHABLE_BANK_END_ADDR)
		{
			// The actual ROM address is dependent on the current rom bank number.
			return ReadFromROM((address - ROM_SWITCHABLE_BANK_START_ADDR) + (ROM_BANK_SIZE * romBankNumber));
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr || !isRAMAndRTCEnabled)
				return 0;

			if (isSimpleRAMBankingMode)
			{
				return ReadFromRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + (RAM_BANK_SIZE * ramBankNumber));
			}
			else
			{
				return rtcRegisters.at(ramBankNumber);
			}
		}

		return 0;
	}

	void MBC3::Write(uint16_t address, uint8_t value)
	{
		if (address >= RAM_AND_TIMER_ENABLE_START_ADDR && address <= RAM_AND_TIMER_ENABLE_END_ADDR)
		{
			// Writing a value with $0A in the lower nibble will enable RAM and the RTC registers, any other value will disable RAM them.
			isRAMAndRTCEnabled = ((value & 0b1111) == 0x0A);
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr || !isRAMAndRTCEnabled)
				return;

			if (isSimpleRAMBankingMode)
			{
				WriteToRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + (RAM_BANK_SIZE * ramBankNumber), value);
			}
			else
			{
				rtcRegisters[ramBankNumber] = value;
			}
		}
		else if (address >= ROM_BANK_NUMBER_START_ADDR && address <= ROM_BANK_NUMBER_END_ADDR)
		{
			// When 0 is written to this address space, the ROM bank number should be incremented to 1 
			// since ROM bank 0 cannot be selected.
			romBankNumber = value == 0 ? 1 : value;
		}
		else if (address >= RAM_BANK_NUMBER_RTC_SELECT_START_ADDR && address <= RAM_BANK_NUMBER_RTC_SELECT_END_ADDR)
		{
			// Writing a value in the range $00 - $03 selects the corresponding RAM bank, 
			// and writing a value between $08 - $0C selects the corresponding RTC register.
			if (Arithmetic::IsInRange(value, 0, 0x03))
			{
				isSimpleRAMBankingMode = true;
			}
			else if (Arithmetic::IsInRange(value, 0x08, 0x0C))
			{
				isSimpleRAMBankingMode = false;
			}

			ramBankNumber = value;
		}
		else if (address >= LATCH_CLOCK_DATA_START_ADDR && address <= LATCH_CLOCK_DATA_END_ADDR)
		{
			// TODO: Day counter is not implemented.
			if (value == 0)
			{
				isLatchingReady = true;
			}
			else if (value == 1)
			{
				SystemTime::DateTime currentTime = SystemTime::GetCurrentTime();

				rtcRegisters[RTC_SECONDS_REGISTER_ADDR] = currentTime.seconds;
				rtcRegisters[RTC_MINUTES_REGISTER_ADDR] = currentTime.minutes;
				rtcRegisters[RTC_HOURS_REGISTER_ADDR] = currentTime.hour;

				isLatchingReady = false;
			}
		}
	}

	void MBC3::Reset()
	{
		MemoryBankController::Reset();

		// The ROM bank number defaults to 1;
		romBankNumber = 1;
		ramBankNumber = 0;

		rtcRegisters =
		{
			{ 0x08, 0 },
			{ 0x09, 0 },
			{ 0x0A, 0 },
			{ 0x0B, 0 },
			{ 0x0C, 0 },
		};
	}
}