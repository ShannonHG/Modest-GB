#include "Graphics/DMATransferRegister.hpp"
namespace SHG
{
	bool DMATransferRegister::IsTransferPending()
	{
		return isTransferPending;
	}

	void DMATransferRegister::ClearPendingTransfer()
	{
		isTransferPending = false;
	}

	uint16_t DMATransferRegister::GetSourceStartAddress()
	{
		return sourceTransferStartAddress;
	}

	uint16_t DMATransferRegister::GetSourceEndAddress()
	{
		return sourceTransferEndAddress;
	}

	uint8_t DMATransferRegister::Read(uint16_t address)
	{
		if (IsAddressAvailable(address))
			return data;
	
		return 0;
	}

	void DMATransferRegister::Write(uint16_t address, uint8_t value)
	{
		if (IsAddressAvailable(address))
		{
			data = value;
			isTransferPending = true;
			sourceTransferStartAddress = value << 8;
			sourceTransferEndAddress = sourceTransferEndAddress | 0x9F;
		}
	}

	bool DMATransferRegister::IsAddressAvailable(uint16_t address)
	{
		// This register only contains a single byte
		return address == 0;
	}

	void DMATransferRegister::Reset()
	{
		data = 0;
		isTransferPending = false;
		sourceTransferStartAddress = 0;
		sourceTransferEndAddress = 0;
	}
}