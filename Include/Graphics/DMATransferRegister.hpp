#pragma once
#include <cstdint>

namespace SHG
{
	class DMATransferRegister
	{
	public:
		bool IsTransferPending() const;
		void ClearPendingTransfer();
		uint16_t GetSourceStartAddress() const;
		uint16_t GetSourceEndAddress() const;
		uint8_t Read() const;
		void Write(uint8_t value);
		void Reset();

	private:
		bool isTransferPending = false;
		uint8_t data = 0;
		uint16_t sourceTransferStartAddress = 0;
		uint16_t sourceTransferEndAddress = 0;
	};
}