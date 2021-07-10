#include "CPU/CPUInstruction.hpp"

namespace SHG
{
	CPUInstruction::CPUInstruction() {}

	CPUInstruction::CPUInstruction(const char* mnemonic, uint8_t cyles, void (CPU::* operation)()) 
		: mnemonic(mnemonic), cycles(cyles), operation(operation)
	{
	}
}