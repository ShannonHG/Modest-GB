#pragma once
#include <cstdint>
#include "Memory/Register8.hpp"

namespace SHG
{
	/// <summary>
	/// Represents a 16-bit register.
	/// </summary>
	class Register16
	{
	public:
		/// <summary>
		/// Returns the data stored in this register.
		/// </summary>
		/// <returns></returns>
		uint16_t GetData();


		/// <summary>
		/// Sets the data in this register to the specified value.
		/// </summary>
		/// <param name="data"></param>
		void SetData(uint16_t data);

		/// <summary>
		/// Enables or disables the bit at the provided position.
		/// </summary>
		/// <param name="bitNumber"></param>
		/// <param name="enabled"></param>
		void SetBit(uint8_t bitNumber, bool enabled);

		/// <summary>
		/// Returns the bit at the provided position.
		/// </summary>
		/// <param name="bitNumber"></param>
		/// <returns></returns>
		uint8_t GetBit(uint8_t bitNumber);

		/// <summary>
		/// Increments this register's data by 1.
		/// </summary>
		void Increment();

		/// <summary>
		/// Decrements this register's data by 1.
		/// </summary>
		void Decrement();

		void Increase(uint16_t amount);
		void Decrease(uint16_t amount);

		/// <summary>
		/// Returns the byte stored in the high 8-bit register.
		/// </summary>
		/// <returns></returns>
		uint8_t GetHighByte();

		/// <summary>
		/// Returns the byte stored in the low 8-bit register.
		/// </summary>
		/// <returns></returns>
		uint8_t GetLowByte();

		/// <summary>
		/// Sets the byte stored in the high 8-bit register to the specified value.
		/// </summary>
		/// <param name="data"></param>
		void SetHighByte(uint8_t data);

		/// <summary>
		/// Sets the byte stored in the low 8-bit register to the specified value.
		/// </summary>
		/// <param name="data"></param>
		void SetLowByte(uint8_t data);
	
		/// <summary>
		/// Returns the high 8-bit register.
		/// </summary>
		/// <returns></returns>
		Register8& GetHighRegister();

		/// <summary>
		/// Returns the low 8-bit register.
		/// </summary>
		/// <returns></returns>
		Register8& GetLowRegister();

	private:
		Register8 highRegister;
		Register8 lowRegister;
	};
}