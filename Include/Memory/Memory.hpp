#pragma once
#include <cstdint>
#include <functional>

namespace ModestGB
{
	class Memory
	{
	public: 
		bool Read(uint16_t address, uint8_t bitNum) const;
		void ChangeBit(uint16_t address, uint8_t bitNum, bool bitValue);
		void SetBit(uint16_t address, uint8_t bitNum);
		void ClearBit(uint16_t address, uint8_t bitNum);
		virtual uint8_t Read(uint16_t address) const = 0;
		virtual void Write(uint16_t address, uint8_t value) = 0;
		virtual void Reset() = 0;
	};
}