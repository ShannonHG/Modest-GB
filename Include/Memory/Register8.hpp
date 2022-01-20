#pragma once
#include <cstdint>
#include "Memory/Memory.hpp"

namespace ModestGB
{
	class Register8
	{
	public:
		uint8_t Read() const;
		void Write(uint8_t data);
		void ChangeBit(uint8_t bitIndex, bool enable);
		uint8_t Read(uint8_t bitIndex) const;
		uint8_t Read(uint8_t start, uint8_t end) const;
		void Fill(bool value);
		void Increment();
		void Decrement();
		void Increase(uint8_t amount);
		void Decrease(uint8_t amount);

	private:
		uint8_t data = 0;
	};
}