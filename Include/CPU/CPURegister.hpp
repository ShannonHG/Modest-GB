#pragma once
#include <cstdint>

namespace SHG
{
	/// <summary>
	/// 16-bit CPU register
	/// </summary>
	class CPURegister
	{
	public: 
		/// <summary>
		/// Sets the upper 8 bits of this register
		/// </summary>
		/// <param name="value"></param>
		void SetUpperByte(uint8_t value);

		/// <summary>
		/// Sets the lower 8 bits of this register
		/// </summary>
		/// <param name="value"></param>
		void SetLowerByte(uint8_t value);

		/// <summary>
		/// Returns the upper 8 bits of this register
		/// </summary>
		/// <returns></returns>
		uint8_t GetUpperByte();

		/// <summary>
		/// Returns the lower 8 bits of this register
		/// </summary>
		/// <returns></returns>
		uint8_t GetLowerByte();

		/// <summary>
		/// Returns the 16 bit value stored in this register
		/// </summary>
		/// <returns></returns>
		uint16_t GetData();

		/// <summary>
		/// Sets the full 16 bits of this register
		/// </summary>
		void SetData(uint16_t value);

		/// <summary>
		/// Increases the stored 16 bit value by 1
		/// </summary>
		void Increment();

		/// <summary>
		/// Decreases the stored 16 bit value by 1
		/// </summary>
		void Decrement();

	private:
		uint16_t data = 0;
	};
}