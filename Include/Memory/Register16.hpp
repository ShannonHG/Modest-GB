#pragma once
#include <cstdint>
#include "Memory/Register8.hpp"

namespace SHG
{
	class Register16 : public Memory
	{
	public:
		Register16();
		uint16_t GetData() const;
		void SetData(uint16_t data);
		void SetBit(uint8_t bitNumber, bool enabled);
		uint8_t GetBit(uint8_t bitNumber) const;
		void Increment();
		void Decrement();
		void Increase(uint16_t amount);
		void Decrease(uint16_t amount);
		uint8_t GetHighByte() const;
		uint8_t GetLowByte() const;
		void SetHighByte(uint8_t data);

		void SetLowByte(uint8_t data);
		Register8& GetHighRegister();
		Register8& GetLowRegister();

		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;
	private:
		Register8 highRegister;
		Register8 lowRegister;
	};
}