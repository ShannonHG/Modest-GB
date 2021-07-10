#pragma once
#include <functional>
#include <string>
#include "Memory/Register16.hpp"

namespace SHG
{
	class CPU;
	struct CPUInstruction
	{
		CPUInstruction();
		CPUInstruction(const char* mnemonic, uint8_t cyles, void (CPU::* operation)());
		const char* mnemonic;
		uint8_t cycles;
		void (CPU::* operation)() = nullptr;
	};
}