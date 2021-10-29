#pragma once
#include <array>
#include "Memory/MemoryBankController.hpp"

namespace SHG
{
	class MBC1 : public MemoryBankController
	{
	public:
		/// <summary>
		/// Returns the current RAM bank number.
		/// </summary>
		/// <returns></returns>
		uint8_t GetRAMBankNumber();

		/// <summary>
		/// Returns the current ROM bank number.
		/// </summary>
		/// <returns></returns>
		uint8_t GetROMBankNumber();

		/// <summary>
		/// Returns the current ROM banking mode.
		/// </summary>
		/// <returns></returns>
		uint8_t GetBankingMode();

		/// <summary>
		/// Returns true if RAM is enabled, and false otherwise.
		/// </summary>
		/// <returns></returns>
		bool IsRAMEnabled();

		/// <inheritdoc/>
		uint8_t Read(uint16_t address) override;

		/// <inheritdoc/>
		void Write(uint16_t address, uint8_t value) override;

		/// <inheritdoc/>
		bool IsAddressAvailable(uint16_t address) override;

	protected:
		std::string GetName() override;

	private:
		std::array<uint8_t, 4> registers{};
	};
}