#pragma once
#include <cstdint>
#include "Memory/DataStorageDevice.hpp"

namespace SHG
{
	class DMATransferRegister : public DataStorageDevice
	{
	public:
		bool IsTransferPending();
		void ClearPendingTransfer();
		uint16_t GetSourceStartAddress();
		uint16_t GetSourceEndAddress();
		uint8_t GetByte(uint16_t address) override;
		void SetByte(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) override;
	
	private:
		bool isTransferPending = false;
		uint8_t data = 0;
		uint16_t sourceTransferStartAddress;
		uint16_t sourceTransferEndAddress;
	};
}