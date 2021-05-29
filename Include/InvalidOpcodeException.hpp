#pragma once
#include <stdexcept>

namespace SHG
{
	/// <summary>
	/// An exception that's thrown when an invalid/unknown opcode is encountered.
	/// </summary>
	class InvalidOpcodeException : public std::runtime_error
	{
	public:
		InvalidOpcodeException(const std::string& message) : std::runtime_error(message) {}
	};
}