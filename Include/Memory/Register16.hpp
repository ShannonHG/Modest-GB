#pragma once
#include <cstdint>
#include "Memory/Register8.hpp"

namespace ModestGB
{
	class Register16
	{
	public:
		Register16();
		uint16_t Read() const;
		void Write(uint16_t data);
		void SetBit(uint8_t bitNumber, bool enabled);
		uint8_t Read(uint8_t bitNumber) const;
		void Increment();
		void Decrement();
		void Increase(uint16_t amount);
		void Decrease(uint16_t amount);
		void Fill(bool value);
		uint8_t GetHighByte() const;
		uint8_t GetLowByte() const;
		void SetHighByte(uint8_t data);

		void SetLowByte(uint8_t data);
		Register8& GetHighRegister();
		Register8& GetLowRegister();
	private:
		Register8 highRegister;
		Register8 lowRegister;
	};
}