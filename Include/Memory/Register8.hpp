#pragma once
#include <cstdint>

namespace SHG
{
	/// <summary>
	/// Represents an 8-bit register.
	/// </summary>
	class Register8
	{
	public:
		/// <summary>
		/// Returns the data stored in this register.
		/// </summary>
		/// <returns></returns>
		uint8_t GetData();

		/// <summary>
		/// Sets the data in this register to the specified value.
		/// </summary>
		/// <param name="data"></param>
		void SetData(uint8_t data);

		/// <summary>
		/// Enables or disables the bit at the provided position.
		/// </summary>
		/// <param name="bitNumber"></param>
		/// <param name="enabled"></param>
		void ChangeBit(uint8_t bitIndex, bool enable);

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

		void Increase(uint8_t amount);
		void Decrease(uint8_t amount);

	protected:
		uint8_t data = 0;
	};
}