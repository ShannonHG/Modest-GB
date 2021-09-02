#include "Memory/DataStorageDevice.hpp"
#include "Common/Arithmetic.hpp"

namespace SHG
{
	bool DataStorageDevice::GetBit(uint16_t address, uint8_t bitNum)
	{
		uint8_t byte = GetByte(address);
		return Arithmetic::GetBit(byte, bitNum);
	}

	void DataStorageDevice::ChangeBit(uint16_t address, uint8_t bitNum, bool bitValue)
	{
		uint8_t byte = GetByte(address);
		Arithmetic::ChangeBit(byte, bitNum, bitValue);
		SetByte(address, byte);
	}

	void DataStorageDevice::SetBit(uint16_t address, uint8_t bitNum)
	{
		ChangeBit(address, bitNum, true);
	}

	void DataStorageDevice::ResetBit(uint16_t address, uint8_t bitNum)
	{
		ChangeBit(address, bitNum, false);
	}
}