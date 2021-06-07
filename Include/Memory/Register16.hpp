#pragma once
#include <cstdint>
#include "Memory/Register8.hpp"

namespace SHG
{
	class Register16
	{
	public:
		uint16_t GetData();
		void SetData(uint16_t data);
		void SetBit(uint8_t bitNumber, bool enabled);
		uint8_t GetBit(uint8_t bitNumber);
		void Increment();
		void Decrement();

		uint8_t GetHighByte();
		uint8_t GetLowByte();
		void SetHighByte(uint8_t data);
		void SetLowByte(uint8_t data);
	
		Register8& GetHighRegister();
		Register8& GetLowRegister();

	private:
		Register8 highRegister;
		Register8 lowRegister;
	};
}