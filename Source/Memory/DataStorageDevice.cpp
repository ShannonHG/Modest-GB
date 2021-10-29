#include "Memory/DataStorageDevice.hpp"
#include "Utils/Arithmetic.hpp"

namespace SHG
{
	bool DataStorageDevice::GetBit(uint16_t address, uint8_t bitNum)
	{
		uint8_t byte = Read(address);
		return Arithmetic::GetBit(byte, bitNum);
	}

	void DataStorageDevice::ChangeBit(uint16_t address, uint8_t bitNum, bool bitValue)
	{
		uint8_t byte = Read(address);
		Arithmetic::ChangeBit(byte, bitNum, bitValue);
		Write(address, byte);
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