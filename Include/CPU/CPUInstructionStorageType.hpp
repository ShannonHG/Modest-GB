#pragma once

namespace SHG
{
	/// <summary>
	/// Specifies where an <see cref="CPUInstruction"/>'s data should be stored.
	/// </summary>
	enum class CPUInstructionStorageType
	{
		/// <summary>
		/// The instruction result should not be stored anywhere.
		/// </summary>
		None,
		/// <summary>
		/// The instruction should cause 8-bit data to be stored in memory.
		/// </summary>
		EightBitMemoryData,
		/// <summary>
		/// The instruction should cause 16-bit data to be stored in memory.
		/// </summary>
		SixteenBitMemoryData,
		/// <summary>
		/// The instruction should cause 8-bit data to be stored in a 16-bit register.
		/// </summary>
		EightBitRegisterData,
		/// <summary>
		/// The instruction should cause 16-bit data to be stored in a 16-bit register.
		/// </summary>
		SixteenBitRegisterData
	};
}