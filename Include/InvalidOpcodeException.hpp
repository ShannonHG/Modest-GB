#pragma once
#include <stdexcept>

namespace SHG
{
	class InvalidOpcodeException : public std::runtime_error
	{
	public:
		InvalidOpcodeException(const std::string& message) : std::runtime_error(message) {}
	};
}