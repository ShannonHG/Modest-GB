#include "Graphics/DMATransferRegister.hpp"

namespace ModestGB
{
	bool DMATransferRegister::IsTransferPending() const
	{
		return isTransferPending;
	}

	void DMATransferRegister::ClearPendingTransfer()
	{
		isTransferPending = false;
	}

	uint16_t DMATransferRegister::GetSourceStartAddress() const
	{
		return sourceTransferStartAddress;
	}

	uint16_t DMATransferRegister::GetSourceEndAddress() const
	{
		return sourceTransferEndAddress;
	}

	uint8_t DMATransferRegister::Read() const
	{
		return data;
	}

	void DMATransferRegister::Write(uint8_t value)
	{
		data = value;
		isTransferPending = true;
		sourceTransferStartAddress = value << 8;
		sourceTransferEndAddress = sourceTransferEndAddress | 0x9F;
	}

	void DMATransferRegister::Reset()
	{
		data = 0;
		isTransferPending = false;
		sourceTransferStartAddress = 0;
		sourceTransferEndAddress = 0;
	}
}