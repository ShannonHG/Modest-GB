#pragma once
#include <cstdint>

namespace SHG
{
	class DataStorageDevice
	{
	public: 
		bool GetBit(uint16_t address, uint8_t bitNum);
		void ChangeBit(uint16_t address, uint8_t bitNum, bool bitValue);
		void SetBit(uint16_t address, uint8_t bitNum);
		void ResetBit(uint16_t address, uint8_t bitNum);

		/// <summary>
		/// Returns the byte at the specified memory address.
		/// </summary>
		/// <param name="address"></param>
		/// <returns></returns>
		virtual uint8_t GetByte(uint16_t address) = 0;

		/// <summary>
		/// Sets the byte at the specified memory address to the provided value.
		/// </summary>
		/// <param name="address"></param>
		/// <param name="value"></param>
		virtual void SetByte(uint16_t address, uint8_t value) = 0;

		/// <summary>
		/// Returns whether the specified memory address can be accessed.
		/// </summary>
		/// <param name="address"></param>
		/// <returns></returns>
		virtual bool IsAddressAvailable(uint16_t address) = 0;
	};
}