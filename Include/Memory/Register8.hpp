#pragma once
#include <cstdint>
#include <Memory/Memory.hpp>

namespace SHG
{
	class Register8 : public Memory
	{
	public:
		Register8();

		uint8_t GetData();
		void SetData(uint8_t data);
		void ChangeBit(uint8_t bitIndex, bool enable);
		uint8_t GetBit(uint8_t bitNumber);
		void Increment();
		void Decrement();
		void Increase(uint8_t amount);
		void Decrease(uint8_t amount);
	};
}