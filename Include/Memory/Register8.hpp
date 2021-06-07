#pragma once
#include <cstdint>

namespace SHG
{
	class Register8
	{
	public:
		uint8_t GetData();
		void SetData(uint8_t data);
		void SetBit(uint8_t bitNumber, bool enabled);
		uint8_t GetBit(uint8_t bitNumber);
		void Increment();
		void Decrement();

	protected:
		uint8_t data;
	};
}