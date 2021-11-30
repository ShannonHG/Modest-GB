#pragma once
#include <cstdint>
#include "Memory/DataStorageDevice.hpp"

namespace SHG
{
	class DMATransferRegister : public DataStorageDevice
	{
	public:
		bool IsTransferPending() const;
		void ClearPendingTransfer();
		uint16_t GetSourceStartAddress() const;
		uint16_t GetSourceEndAddress() const;
		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;
	private:
		bool isTransferPending = false;
		uint8_t data = 0;
		uint16_t sourceTransferStartAddress = 0;
		uint16_t sourceTransferEndAddress = 0;
	};
}