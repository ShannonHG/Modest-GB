#include <cassert>
#include <string>
#include <iomanip>
#include <variant>
#include "Utils/DataConversions.hpp"
#include "CPU/CPU.hpp"
#include "Logger.hpp"
#include "InvalidOpcodeException.hpp"
#include "Utils/Arithmetic.hpp"
#include "CPU/Interrupts.hpp"

namespace SHG
{
	const std::string CPU_MESSAGE_HEADER = "[CPU]";
	const uint16_t REGISTER_AF_DEFAULT = 0x01B0;
	const uint16_t REGISTER_BC_DEFAULT = 0x0013;
	const uint16_t REGISTER_DE_DEFAULT = 0x00D8;
	const uint16_t REGISTER_HL_DEFAULT = 0x014D;
	const uint16_t STACK_POINTER_DEFAULT = 0xFFFE;
	const uint16_t PROGRAM_COUNTER_DEFAULT = 0x0100;

	const uint8_t HALT_CYCLE_COUNT = 4;

	CPU::CPU(DataStorageDevice& memoryManagementUnit) : memoryManagementUnit(memoryManagementUnit)
	{
		this->memoryManagementUnit = memoryManagementUnit;
	}

	void CPU::PrintRegisterInfo()
	{
		Logger::WriteSystemEvent("(z) " + std::to_string((int)GetZeroFlag()) +
			" (n) " + std::to_string((int)GetSubtractionFlag()) +
			" (h) " + std::to_string((int)GetHalfCarryFlag()) +
			" (c) " + std::to_string((int)GetCarryFlag()), CPU_MESSAGE_HEADER);

		Logger::WriteSystemEvent("(A) " + ConvertToHexString(GetRegisterA().GetData(), 2) +
			" (F) " + ConvertToHexString(GetRegisterF().GetData(), 2) +
			" (B) " + ConvertToHexString(GetRegisterB().GetData(), 2) +
			" (C) " + ConvertToHexString(GetRegisterC().GetData(), 2), CPU_MESSAGE_HEADER);

		Logger::WriteSystemEvent("(D) " + ConvertToHexString(GetRegisterD().GetData(), 2) +
			" (E) " + ConvertToHexString(GetRegisterE().GetData(), 2) +
			" (H) " + ConvertToHexString(GetRegisterH().GetData(), 2) +
			" (L) " + ConvertToHexString(GetRegisterL().GetData(), 2), CPU_MESSAGE_HEADER);

		Logger::WriteSystemEvent("(PC) " + ConvertToHexString(programCounter.GetData(), 4) +
			" (SP) " + ConvertToHexString(stackPointer.GetData(), 4), CPU_MESSAGE_HEADER);
	}

	void CPU::HandleInterrupts()
	{
		uint8_t interruptFlag = memoryManagementUnit.Read(INTERRUPT_FLAG_ADDRESS);
		uint8_t interruptEnable = memoryManagementUnit.Read(INTERRUPT_ENABLE_ADDRESS);

		if (Logger::IsSystemEventLoggingEnabled)
		{
			Logger::WriteSystemEvent("(IME) " + std::to_string((int)GetInterruptMasterEnableFlag()) +
				" (IF) " + ConvertToHexString(interruptFlag, 2) +
				" (IE) " + ConvertToHexString(interruptEnable, 2), CPU_MESSAGE_HEADER);
		}

		for (int i = 0; i < INTERRUPT_OPERATIONS.size(); i++)
		{
			// If the bit with the current index is set in both the flag and enable registers, 
			// then perform the relevant operation, and reset all interrupt related registers.
			if (((interruptFlag >> i) & 1) == 1 && ((interruptEnable >> i) & 1) == 1)
			{
				uint8_t interruptAddress = INTERRUPT_OPERATIONS.at(static_cast<InterruptType>(i));

				if (Logger::IsSystemEventLoggingEnabled)
					Logger::WriteSystemEvent("(interrupt request) " + ConvertToHexString(interruptAddress, 2), CPU_MESSAGE_HEADER);

				if (interruptMasterEnableFlag)
				{
					CALL(interruptAddress);
					interruptMasterEnableFlag = false;
					memoryManagementUnit.ResetBit(INTERRUPT_FLAG_ADDRESS, i);
				}

				isHalted = false;
				return;
			}
		}
	}

	void CPU::Reset()
	{
		regAF.SetData(REGISTER_AF_DEFAULT);
		regBC.SetData(REGISTER_BC_DEFAULT);
		regDE.SetData(REGISTER_DE_DEFAULT);
		regHL.SetData(REGISTER_HL_DEFAULT);
		stackPointer.SetData(STACK_POINTER_DEFAULT);
		programCounter.SetData(PROGRAM_COUNTER_DEFAULT);

		isHalted = false;
		currentCycles = 0;
		currentInstruction = nullptr;
		interruptMasterEnableFlag = false;
	}

	uint32_t CPU::Step()
	{
		//currentInstruction = nullptr;
		currentCycles = 0;

		if (isHalted)
		{
			currentCycles = HALT_CYCLE_COUNT;
			return currentCycles;
		}

		if (Logger::IsSystemEventLoggingEnabled)
			PrintRegisterInfo();

		uint8_t opcode = Fetch8();
		currentInstruction = Decode(opcode);

		if (currentInstruction == nullptr)
			throw InvalidOpcodeException("[CPU] Invalid opcode encountered: " + ConvertToHexString(opcode, 2));

		currentCycles = currentInstruction->cycles;

		if (Logger::IsSystemEventLoggingEnabled)
			Logger::WriteSystemEvent("(execute) " + std::string(currentInstruction->mnemonic), CPU_MESSAGE_HEADER);

		(this->*(currentInstruction->operation))();

		return currentCycles;
	}

	uint8_t CPU::Fetch8()
	{
		// Get the memory address of the byte to fetch, 
		// and point the program counter to the next address.
		uint16_t address = programCounter.GetData();
		programCounter.Increment();

		// If, for some reason, the memory address is not available
		// then execution should be stopped since this indicates a severe issue.
		if (!memoryManagementUnit.IsAddressAvailable(address))
			throw std::out_of_range("[CPU] Failed to fetch data from " + ConvertToHexString(address, 4) + ". Memory address is inaccessible.");

		uint8_t result = memoryManagementUnit.Read(address);

		if (Logger::IsSystemEventLoggingEnabled)
			Logger::WriteSystemEvent("(fetch) " + ConvertToHexString(result, 2), CPU_MESSAGE_HEADER);

		return result;
	}

	uint16_t CPU::Fetch16()
	{
		uint8_t lowerByte = Fetch8();
		uint8_t upperByte = Fetch8();

		return (upperByte << 8) | lowerByte;
	}

	CPUInstruction* CPU::Decode(uint8_t opcode)
	{
		if (opcode == 0xCB)
		{
			opcode = Fetch8();
			if (CBPrefixedInstructionSet.find(opcode) != CBPrefixedInstructionSet.end())
			{
				return &CBPrefixedInstructionSet.at(opcode);
			}

			return nullptr;
		}
		else if (BasicInstructionSet.find(opcode) != BasicInstructionSet.end())
		{
			return &BasicInstructionSet[opcode];
		}
		else return nullptr;
	}

	Register8& CPU::GetRegisterA()
	{
		return regAF.GetHighRegister();
	}
	Register8& CPU::GetRegisterF()
	{
		return regAF.GetLowRegister();
	}

	Register8& CPU::GetRegisterB()
	{
		return regBC.GetHighRegister();
	}

	Register8& CPU::GetRegisterC()
	{
		return regBC.GetLowRegister();
	}

	Register8& CPU::GetRegisterD()
	{
		return regDE.GetHighRegister();
	}

	Register8& CPU::GetRegisterE()
	{
		return regDE.GetLowRegister();
	}

	Register8& CPU::GetRegisterH()
	{
		return regHL.GetHighRegister();
	}

	Register8& CPU::GetRegisterL()
	{
		return regHL.GetLowRegister();
	}

	Register16& CPU::GetRegisterAF()
	{
		return regAF;
	}

	Register16& CPU::GetRegisterBC()
	{
		return regBC;
	}

	Register16& CPU::GetRegisterDE()
	{
		return regDE;
	}

	Register16& CPU::GetRegisterHL()
	{
		return regHL;
	}

	Register16& CPU::GetProgramCounter()
	{
		return programCounter;
	}

	Register16& CPU::GetStackPointer()
	{
		return stackPointer;
	}

	uint8_t CPU::GetZeroFlag()
	{
		return regAF.GetLowRegister().GetBit(7);
	}

	uint8_t CPU::GetSubtractionFlag()
	{
		return regAF.GetLowRegister().GetBit(6);
	}

	uint8_t CPU::GetHalfCarryFlag()
	{
		return regAF.GetLowRegister().GetBit(5);
	}

	uint8_t CPU::GetCarryFlag()
	{
		return regAF.GetLowRegister().GetBit(4);
	}

	void CPU::ChangeZeroFlag(bool isSet)
	{
		regAF.GetLowRegister().ChangeBit(7, isSet);
	}

	void CPU::ChangeSubtractionFlag(bool isSet)
	{
		regAF.GetLowRegister().ChangeBit(6, isSet);
	}

	void CPU::ChangeHalfCarryFlag(bool isSet)
	{
		regAF.GetLowRegister().ChangeBit(5, isSet);
	}

	void CPU::ChangeCarryFlag(bool isSet)
	{
		regAF.GetLowRegister().ChangeBit(4, isSet);
	}

	void CPU::Set16BitDataInMemory(uint16_t address, uint16_t data)
	{
		memoryManagementUnit.Write(address, data & 0x00FF);
		memoryManagementUnit.Write(address + 1, data >> 8);
	}

	uint16_t CPU::Get16BitDataFromMemory(uint16_t address)
	{
		uint8_t lower = memoryManagementUnit.Read(address);
		uint8_t upper = memoryManagementUnit.Read(address + 1);

		return (upper << 8) | lower;
	}

	CPUInstruction CPU::GetCurrentInstruction()
	{
		return *currentInstruction;
	}

	bool CPU::IsPreviousInstructionValid()
	{
		return currentInstruction != nullptr;
	}

	bool CPU::GetInterruptMasterEnableFlag()
	{
		return interruptMasterEnableFlag;
	}

	void CPU::ChangeInterruptMasterEnableFlag(bool enable)
	{
		interruptMasterEnableFlag = enable;
	}

	std::map<uint8_t, CPUInstruction> CPU::BasicInstructionSet =
	{
		{ 0x00, CPUInstruction("NOP", 4, &CPU::Execute00) },
		{ 0x01, CPUInstruction("LD BC, U16", 12, &CPU::Execute01) },
		{ 0x02, CPUInstruction("LD (BC), A", 8, &CPU::Execute02) },
		{ 0x03, CPUInstruction("INC BC", 8, &CPU::Execute03) },
		{ 0x04, CPUInstruction("INC B", 4, &CPU::Execute04) },
		{ 0x05, CPUInstruction("DEC B", 4, &CPU::Execute05) },
		{ 0x06, CPUInstruction("LD B, U8", 8, &CPU::Execute06) },
		{ 0x07, CPUInstruction("RLCA", 4, &CPU::Execute07) },
		{ 0x08, CPUInstruction("LD (U16), SP", 20, &CPU::Execute08) },
		{ 0x09, CPUInstruction("ADD HL, BC", 8, &CPU::Execute09) },
		{ 0x0A, CPUInstruction("LD A, (BC)", 8, &CPU::Execute0A) },
		{ 0x0B, CPUInstruction("DEC BC", 8, &CPU::Execute0B) },
		{ 0x0C, CPUInstruction("INC C", 4, &CPU::Execute0C) },
		{ 0x0D, CPUInstruction("DEC C", 4, &CPU::Execute0D) },
		{ 0x0E, CPUInstruction("LD C, U8", 8, &CPU::Execute0E) },
		{ 0x0F, CPUInstruction("RRCA", 4, &CPU::Execute0F) },
		{ 0x10, CPUInstruction("STOP", 4, &CPU::Execute10) },
		{ 0x11, CPUInstruction("LD DE, U16", 12, &CPU::Execute11) },
		{ 0x12, CPUInstruction("LD (DE), A", 8, &CPU::Execute12) },
		{ 0x13, CPUInstruction("INC DE", 8, &CPU::Execute13) },
		{ 0x14, CPUInstruction("INC D", 4, &CPU::Execute14) },
		{ 0x15, CPUInstruction("DEC D", 4, &CPU::Execute15) },
		{ 0x16, CPUInstruction("LD D, U8", 8, &CPU::Execute16) },
		{ 0x17, CPUInstruction("RLA", 4, &CPU::Execute17) },
		{ 0x18, CPUInstruction("JR I8", 12, &CPU::Execute18) },
		{ 0x19, CPUInstruction("ADD HL, DE", 8, &CPU::Execute19) },
		{ 0x1A, CPUInstruction("LD A, (DE)", 8, &CPU::Execute1A) },
		{ 0x1B, CPUInstruction("DEC DE", 8, &CPU::Execute1B) },
		{ 0x1C, CPUInstruction("INC E", 4, &CPU::Execute1C) },
		{ 0x1D, CPUInstruction("DEC E", 4, &CPU::Execute1D) },
		{ 0x1E, CPUInstruction("LD E, U8", 8, &CPU::Execute1E) },
		{ 0x1F, CPUInstruction("RRA", 4, &CPU::Execute1F) },
		{ 0x20, CPUInstruction("JR NZ, I8", 8, &CPU::Execute20) },
		{ 0x21, CPUInstruction("LD HL, U16", 12, &CPU::Execute21) },
		{ 0x22, CPUInstruction("LD (HL+), A", 8, &CPU::Execute22) },
		{ 0x23, CPUInstruction("INC HL", 8, &CPU::Execute23) },
		{ 0x24, CPUInstruction("INC H", 4, &CPU::Execute24) },
		{ 0x25, CPUInstruction("DEC H", 4, &CPU::Execute25) },
		{ 0x26, CPUInstruction("LD H, U8", 8, &CPU::Execute26) },
		{ 0x27, CPUInstruction("DAA", 4, &CPU::Execute27) },
		{ 0x28, CPUInstruction("JR Z, I8", 8, &CPU::Execute28) },
		{ 0x29, CPUInstruction("ADD HL, HL", 8, &CPU::Execute29) },
		{ 0x2A, CPUInstruction("LD A, (HL+)", 8, &CPU::Execute2A) },
		{ 0x2B, CPUInstruction("DEC HL", 8, &CPU::Execute2B) },
		{ 0x2C, CPUInstruction("INC L", 4, &CPU::Execute2C) },
		{ 0x2D, CPUInstruction("DEC L", 4, &CPU::Execute2D) },
		{ 0x2E, CPUInstruction("LD L, U8", 8, &CPU::Execute2E) },
		{ 0x2F, CPUInstruction("CPL", 4, &CPU::Execute2F) },
		{ 0x30, CPUInstruction("JR NC, I8", 8, &CPU::Execute30) },
		{ 0x31, CPUInstruction("LD SP, U16", 12, &CPU::Execute31) },
		{ 0x32, CPUInstruction("LD (HL-), A", 8, &CPU::Execute32) },
		{ 0x33, CPUInstruction("INC SP", 8, &CPU::Execute33) },
		{ 0x34, CPUInstruction("INC (HL)", 12, &CPU::Execute34) },
		{ 0x35, CPUInstruction("DEC (HL)", 12, &CPU::Execute35) },
		{ 0x36, CPUInstruction("LD (HL), U8", 12, &CPU::Execute36) },
		{ 0x37, CPUInstruction("SCF", 4, &CPU::Execute37) },
		{ 0x38, CPUInstruction("JR C, I8", 8, &CPU::Execute38) },
		{ 0x39, CPUInstruction("ADD HL, SP", 8, &CPU::Execute39) },
		{ 0x3A, CPUInstruction("LD A, (HL-)", 8, &CPU::Execute3A) },
		{ 0x3B, CPUInstruction("DEC SP", 8, &CPU::Execute3B) },
		{ 0x3C, CPUInstruction("INC A", 4, &CPU::Execute3C) },
		{ 0x3D, CPUInstruction("DEC A", 4, &CPU::Execute3D) },
		{ 0x3E, CPUInstruction("LD A, U8", 8, &CPU::Execute3E) },
		{ 0x3F, CPUInstruction("CCF", 4, &CPU::Execute3F) },
		{ 0x40, CPUInstruction("LD B, B", 4, &CPU::Execute40) },
		{ 0x41, CPUInstruction("LD B, C", 4, &CPU::Execute41) },
		{ 0x42, CPUInstruction("LD B, D", 4, &CPU::Execute42) },
		{ 0x43, CPUInstruction("LD B, E", 4, &CPU::Execute43) },
		{ 0x44, CPUInstruction("LD B, H", 4, &CPU::Execute44) },
		{ 0x45, CPUInstruction("LD B, L", 4, &CPU::Execute45) },
		{ 0x46, CPUInstruction("LD B, (HL)", 8, &CPU::Execute46) },
		{ 0x47, CPUInstruction("LD B, A", 4, &CPU::Execute47) },
		{ 0x48, CPUInstruction("LD C, B", 4, &CPU::Execute48) },
		{ 0x49, CPUInstruction("LD C, C", 4, &CPU::Execute49) },
		{ 0x4A, CPUInstruction("LD C, D", 4, &CPU::Execute4A) },
		{ 0x4B, CPUInstruction("LD C, E", 4, &CPU::Execute4B) },
		{ 0x4C, CPUInstruction("LD C, H", 4, &CPU::Execute4C) },
		{ 0x4D, CPUInstruction("LD C, L", 4, &CPU::Execute4D) },
		{ 0x4E, CPUInstruction("LD C, (HL)", 8, &CPU::Execute4E) },
		{ 0x4F, CPUInstruction("LD C, A", 4, &CPU::Execute4F) },
		{ 0x50, CPUInstruction("LD D, B", 4, &CPU::Execute50) },
		{ 0x51, CPUInstruction("LD D, C", 4, &CPU::Execute51) },
		{ 0x52, CPUInstruction("LD D, D", 4, &CPU::Execute52) },
		{ 0x53, CPUInstruction("LD D, E", 4, &CPU::Execute53) },
		{ 0x54, CPUInstruction("LD D, H", 4, &CPU::Execute54) },
		{ 0x55, CPUInstruction("LD D, L", 4, &CPU::Execute55) },
		{ 0x56, CPUInstruction("LD D, (HL)", 8, &CPU::Execute56) },
		{ 0x57, CPUInstruction("LD D, A", 4, &CPU::Execute57) },
		{ 0x58, CPUInstruction("LD E, B", 4, &CPU::Execute58) },
		{ 0x59, CPUInstruction("LD E, C", 4, &CPU::Execute59) },
		{ 0x5A, CPUInstruction("LD E, D", 4, &CPU::Execute5A) },
		{ 0x5B, CPUInstruction("LD E, E", 4, &CPU::Execute5B) },
		{ 0x5C, CPUInstruction("LD E, H", 4, &CPU::Execute5C) },
		{ 0x5D, CPUInstruction("LD E, L", 4, &CPU::Execute5D) },
		{ 0x5E, CPUInstruction("LD E, (HL)", 8, &CPU::Execute5E) },
		{ 0x5F, CPUInstruction("LD E, A", 4, &CPU::Execute5F) },
		{ 0x60, CPUInstruction("LD H, B", 4, &CPU::Execute60) },
		{ 0x61, CPUInstruction("LD H, C", 4, &CPU::Execute61) },
		{ 0x62, CPUInstruction("LD H, D", 4, &CPU::Execute62) },
		{ 0x63, CPUInstruction("LD H, E", 4, &CPU::Execute63) },
		{ 0x64, CPUInstruction("LD H, H", 4, &CPU::Execute64) },
		{ 0x65, CPUInstruction("LD H, L", 4, &CPU::Execute65) },
		{ 0x66, CPUInstruction("LD H, (HL)", 8, &CPU::Execute66) },
		{ 0x67, CPUInstruction("LD H, A", 4, &CPU::Execute67) },
		{ 0x68, CPUInstruction("LD L, B", 4, &CPU::Execute68) },
		{ 0x69, CPUInstruction("LD L, C", 4, &CPU::Execute69) },
		{ 0x6A, CPUInstruction("LD L, D", 4, &CPU::Execute6A) },
		{ 0x6B, CPUInstruction("LD L, E", 4, &CPU::Execute6B) },
		{ 0x6C, CPUInstruction("LD L, H", 4, &CPU::Execute6C) },
		{ 0x6D, CPUInstruction("LD L, L", 4, &CPU::Execute6D) },
		{ 0x6E, CPUInstruction("LD L, (HL)", 8, &CPU::Execute6E) },
		{ 0x6F, CPUInstruction("LD L, A", 4, &CPU::Execute6F) },
		{ 0x70, CPUInstruction("LD (HL), B", 8, &CPU::Execute70) },
		{ 0x71, CPUInstruction("LD (HL), C", 8, &CPU::Execute71) },
		{ 0x72, CPUInstruction("LD (HL), D", 8, &CPU::Execute72) },
		{ 0x73, CPUInstruction("LD (HL), E", 8, &CPU::Execute73) },
		{ 0x74, CPUInstruction("LD (HL), H", 8, &CPU::Execute74) },
		{ 0x75, CPUInstruction("LD (HL), L", 8, &CPU::Execute75) },
		{ 0x76, CPUInstruction("HALT", 4, &CPU::Execute76) },
		{ 0x77, CPUInstruction("LD (HL), A", 8, &CPU::Execute77) },
		{ 0x78, CPUInstruction("LD A, B", 4, &CPU::Execute78) },
		{ 0x79, CPUInstruction("LD A, C", 4, &CPU::Execute79) },
		{ 0x7A, CPUInstruction("LD A, D", 4, &CPU::Execute7A) },
		{ 0x7B, CPUInstruction("LD A, E", 4, &CPU::Execute7B) },
		{ 0x7C, CPUInstruction("LD A, H", 4, &CPU::Execute7C) },
		{ 0x7D, CPUInstruction("LD A, L", 4, &CPU::Execute7D) },
		{ 0x7E, CPUInstruction("LD A, (HL)", 8, &CPU::Execute7E) },
		{ 0x7F, CPUInstruction("LD A, A", 4, &CPU::Execute7F) },
		{ 0x80, CPUInstruction("ADD A, B", 4, &CPU::Execute80) },
		{ 0x81, CPUInstruction("ADD A, C", 4, &CPU::Execute81) },
		{ 0x82, CPUInstruction("ADD A, D", 4, &CPU::Execute82) },
		{ 0x83, CPUInstruction("ADD A, E", 4, &CPU::Execute83) },
		{ 0x84, CPUInstruction("ADD A, H", 4, &CPU::Execute84) },
		{ 0x85, CPUInstruction("ADD A, L", 4, &CPU::Execute85) },
		{ 0x86, CPUInstruction("ADD A, (HL)", 8, &CPU::Execute86) },
		{ 0x87, CPUInstruction("ADD A, A", 4, &CPU::Execute87) },
		{ 0x88, CPUInstruction("ADC A, B", 4, &CPU::Execute88) },
		{ 0x89, CPUInstruction("ADC A, C", 4, &CPU::Execute89) },
		{ 0x8A, CPUInstruction("ADC A, D", 4, &CPU::Execute8A) },
		{ 0x8B, CPUInstruction("ADC A, E", 4, &CPU::Execute8B) },
		{ 0x8C, CPUInstruction("ADC A, H", 4, &CPU::Execute8C) },
		{ 0x8D, CPUInstruction("ADC A, L", 4, &CPU::Execute8D) },
		{ 0x8E, CPUInstruction("ADC A, (HL)", 8, &CPU::Execute8E) },
		{ 0x8F, CPUInstruction("ADC A, A", 4, &CPU::Execute8F) },
		{ 0x90, CPUInstruction("SUB A, B", 4, &CPU::Execute90) },
		{ 0x91, CPUInstruction("SUB A, C", 4, &CPU::Execute91) },
		{ 0x92, CPUInstruction("SUB A, D", 4, &CPU::Execute92) },
		{ 0x93, CPUInstruction("SUB A, E", 4, &CPU::Execute93) },
		{ 0x94, CPUInstruction("SUB A, H", 4, &CPU::Execute94) },
		{ 0x95, CPUInstruction("SUB A, L", 4, &CPU::Execute95) },
		{ 0x96, CPUInstruction("SUB A, (HL)", 8, &CPU::Execute96) },
		{ 0x97, CPUInstruction("SUB A, A", 4, &CPU::Execute97) },
		{ 0x98, CPUInstruction("SBC A, B", 4, &CPU::Execute98) },
		{ 0x99, CPUInstruction("SBC A, C", 4, &CPU::Execute99) },
		{ 0x9A, CPUInstruction("SBC A, D", 4, &CPU::Execute9A) },
		{ 0x9B, CPUInstruction("SBC A, E", 4, &CPU::Execute9B) },
		{ 0x9C, CPUInstruction("SBC A, H", 4, &CPU::Execute9C) },
		{ 0x9D, CPUInstruction("SBC A, L", 4, &CPU::Execute9D) },
		{ 0x9E, CPUInstruction("SBC A, (HL)", 8, &CPU::Execute9E) },
		{ 0x9F, CPUInstruction("SBC A, A", 4, &CPU::Execute9F) },
		{ 0xA0, CPUInstruction("AND A, B", 4, &CPU::ExecuteA0) },
		{ 0xA1, CPUInstruction("AND A, C", 4, &CPU::ExecuteA1) },
		{ 0xA2, CPUInstruction("AND A, D", 4, &CPU::ExecuteA2) },
		{ 0xA3, CPUInstruction("AND A, E", 4, &CPU::ExecuteA3) },
		{ 0xA4, CPUInstruction("AND A, H", 4, &CPU::ExecuteA4) },
		{ 0xA5, CPUInstruction("AND A, L", 4, &CPU::ExecuteA5) },
		{ 0xA6, CPUInstruction("AND A, (HL)", 8, &CPU::ExecuteA6) },
		{ 0xA7, CPUInstruction("AND A, A", 4, &CPU::ExecuteA7) },
		{ 0xA8, CPUInstruction("XOR A, B", 4, &CPU::ExecuteA8) },
		{ 0xA9, CPUInstruction("XOR A, C", 4, &CPU::ExecuteA9) },
		{ 0xAA, CPUInstruction("XOR A, D", 4, &CPU::ExecuteAA) },
		{ 0xAB, CPUInstruction("XOR A, E", 4, &CPU::ExecuteAB) },
		{ 0xAC, CPUInstruction("XOR A, H", 4, &CPU::ExecuteAC) },
		{ 0xAD, CPUInstruction("XOR A, L", 4, &CPU::ExecuteAD) },
		{ 0xAE, CPUInstruction("XOR A, (HL)", 8, &CPU::ExecuteAE) },
		{ 0xAF, CPUInstruction("XOR A, A", 4, &CPU::ExecuteAF) },
		{ 0xB0, CPUInstruction("OR A, B", 4, &CPU::ExecuteB0) },
		{ 0xB1, CPUInstruction("OR A, C", 4, &CPU::ExecuteB1) },
		{ 0xB2, CPUInstruction("OR A, D", 4, &CPU::ExecuteB2) },
		{ 0xB3, CPUInstruction("OR A, E", 4, &CPU::ExecuteB3) },
		{ 0xB4, CPUInstruction("OR A, H", 4, &CPU::ExecuteB4) },
		{ 0xB5, CPUInstruction("OR A, L", 4, &CPU::ExecuteB5) },
		{ 0xB6, CPUInstruction("OR A, (HL)", 8, &CPU::ExecuteB6) },
		{ 0xB7, CPUInstruction("OR A, A", 4, &CPU::ExecuteB7) },
		{ 0xB8, CPUInstruction("CP A, B", 4, &CPU::ExecuteB8) },
		{ 0xB9, CPUInstruction("CP A, C", 4, &CPU::ExecuteB9) },
		{ 0xBA, CPUInstruction("CP A, D", 4, &CPU::ExecuteBA) },
		{ 0xBB, CPUInstruction("CP A, E", 4, &CPU::ExecuteBB) },
		{ 0xBC, CPUInstruction("CP A, H", 4, &CPU::ExecuteBC) },
		{ 0xBD, CPUInstruction("CP A, L", 4, &CPU::ExecuteBD) },
		{ 0xBE, CPUInstruction("CP A, (HL)", 8, &CPU::ExecuteBE) },
		{ 0xBF, CPUInstruction("CP A, A", 4, &CPU::ExecuteBF) },
		{ 0xC0, CPUInstruction("RET NZ", 8, &CPU::ExecuteC0) },
		{ 0xC1, CPUInstruction("POP BC", 12, &CPU::ExecuteC1) },
		{ 0xC2, CPUInstruction("JP NZ, U16", 12, &CPU::ExecuteC2) },
		{ 0xC3, CPUInstruction("JP U16", 16, &CPU::ExecuteC3) },
		{ 0xC4, CPUInstruction("CALL NZ, U16", 12, &CPU::ExecuteC4) },
		{ 0xC5, CPUInstruction("PUSH BC", 16, &CPU::ExecuteC5) },
		{ 0xC6, CPUInstruction("ADD A, U8", 8, &CPU::ExecuteC6) },
		{ 0xC7, CPUInstruction("RST 00h", 16, &CPU::ExecuteC7) },
		{ 0xC8, CPUInstruction("RET Z", 8, &CPU::ExecuteC8) },
		{ 0xC9, CPUInstruction("RET", 16, &CPU::ExecuteC9) },
		{ 0xCA, CPUInstruction("JP Z, U16", 12, &CPU::ExecuteCA) },
		{ 0xCC, CPUInstruction("CALL Z, U16", 12, &CPU::ExecuteCC) },
		{ 0xCD, CPUInstruction("CALL U16", 24, &CPU::ExecuteCD) },
		{ 0xCE, CPUInstruction("ADC A, U8", 8, &CPU::ExecuteCE) },
		{ 0xCF, CPUInstruction("RST 08h", 16, &CPU::ExecuteCF) },
		{ 0xD0, CPUInstruction("RET NC", 8, &CPU::ExecuteD0) },
		{ 0xD1, CPUInstruction("POP DE", 12, &CPU::ExecuteD1) },
		{ 0xD2, CPUInstruction("JP NC, U16", 12, &CPU::ExecuteD2) },
		{ 0xD4, CPUInstruction("CALL NC, U16", 12, &CPU::ExecuteD4) },
		{ 0xD5, CPUInstruction("PUSH DE", 16, &CPU::ExecuteD5) },
		{ 0xD6, CPUInstruction("SUB A, U8", 8, &CPU::ExecuteD6) },
		{ 0xD7, CPUInstruction("RST 10h", 16, &CPU::ExecuteD7) },
		{ 0xD8, CPUInstruction("RET C", 8, &CPU::ExecuteD8) },
		{ 0xD9, CPUInstruction("RETI", 16, &CPU::ExecuteD9) },
		{ 0xDA, CPUInstruction("JP C, U16", 12, &CPU::ExecuteDA) },
		{ 0xDC, CPUInstruction("CALL C, U16", 12, &CPU::ExecuteDC) },
		{ 0xDE, CPUInstruction("SBC A, U8", 8, &CPU::ExecuteDE) },
		{ 0xDF, CPUInstruction("RST 18h", 16, &CPU::ExecuteDF) },
		{ 0xE0, CPUInstruction("LD (FF00 + U8), A", 12, &CPU::ExecuteE0) },
		{ 0xE1, CPUInstruction("POP HL", 12, &CPU::ExecuteE1) },
		{ 0xE2, CPUInstruction("LD (FF00 + C), A", 8, &CPU::ExecuteE2) },
		{ 0xE5, CPUInstruction("PUSH HL", 16, &CPU::ExecuteE5) },
		{ 0xE6, CPUInstruction("AND A, U8", 8, &CPU::ExecuteE6) },
		{ 0xE7, CPUInstruction("RST 20h", 16, &CPU::ExecuteE7) },
		{ 0xE8, CPUInstruction("ADD SP I8", 16, &CPU::ExecuteE8) },
		{ 0xE9, CPUInstruction("JP HL", 4, &CPU::ExecuteE9) },
		{ 0xEA, CPUInstruction("LD (U16), A", 16, &CPU::ExecuteEA) },
		{ 0xEE, CPUInstruction("XOR A, U8", 8, &CPU::ExecuteEE) },
		{ 0xEF, CPUInstruction("RST 28h", 16, &CPU::ExecuteEF) },
		{ 0xF0, CPUInstruction("LD A, (FF00 + U8)", 12, &CPU::ExecuteF0) },
		{ 0xF1, CPUInstruction("POP AF", 12, &CPU::ExecuteF1) },
		{ 0xF2, CPUInstruction("LD A, (FF00 + C)", 8, &CPU::ExecuteF2) },
		{ 0xF3, CPUInstruction("DI", 4, &CPU::ExecuteF3) },
		{ 0xF5, CPUInstruction("PUSH AF", 16, &CPU::ExecuteF5) },
		{ 0xF6, CPUInstruction("OR A, U8", 8, &CPU::ExecuteF6) },
		{ 0xF7, CPUInstruction("RST 30h", 16, &CPU::ExecuteF7) },
		{ 0xF8, CPUInstruction("LD HL, SP + I8", 12, &CPU::ExecuteF8) },
		{ 0xF9, CPUInstruction("LD SP, HL", 8, &CPU::ExecuteF9) },
		{ 0xFA, CPUInstruction("LD A, (U16)", 16, &CPU::ExecuteFA) },
		{ 0xFB, CPUInstruction("EI", 4, &CPU::ExecuteFB) },
		{ 0xFE, CPUInstruction("CP A, U8", 8, &CPU::ExecuteFE) },
		{ 0xFF, CPUInstruction("RST 38h", 16, &CPU::ExecuteFF) }
	};

	std::map<uint8_t, CPUInstruction> CPU::CBPrefixedInstructionSet =
	{
		{ 0x00, CPUInstruction("RLC B", 8, &CPU::ExecuteCB00) },
		{ 0x01, CPUInstruction("RLC C", 8, &CPU::ExecuteCB01) },
		{ 0x02, CPUInstruction("RLC D", 8, &CPU::ExecuteCB02) },
		{ 0x03, CPUInstruction("RLC E", 8, &CPU::ExecuteCB03) },
		{ 0x04, CPUInstruction("RLC H", 8, &CPU::ExecuteCB04) },
		{ 0x05, CPUInstruction("RLC L", 8, &CPU::ExecuteCB05) },
		{ 0x06, CPUInstruction("RLC (HL)", 16, &CPU::ExecuteCB06) },
		{ 0x07, CPUInstruction("RLC A", 8, &CPU::ExecuteCB07) },
		{ 0x08, CPUInstruction("RRC B", 8, &CPU::ExecuteCB08) },
		{ 0x09, CPUInstruction("RRC C", 8, &CPU::ExecuteCB09) },
		{ 0x0A, CPUInstruction("RRC D", 8, &CPU::ExecuteCB0A) },
		{ 0x0B, CPUInstruction("RRC E", 8, &CPU::ExecuteCB0B) },
		{ 0x0C, CPUInstruction("RRC H", 8, &CPU::ExecuteCB0C) },
		{ 0x0D, CPUInstruction("RRC L", 8, &CPU::ExecuteCB0D) },
		{ 0x0E, CPUInstruction("RRC (HL)", 16, &CPU::ExecuteCB0E) },
		{ 0x0F, CPUInstruction("RRC A", 8, &CPU::ExecuteCB0F) },
		{ 0x10, CPUInstruction("RL B", 8, &CPU::ExecuteCB10) },
		{ 0x11, CPUInstruction("RL C", 8, &CPU::ExecuteCB11) },
		{ 0x12, CPUInstruction("RL D", 8, &CPU::ExecuteCB12) },
		{ 0x13, CPUInstruction("RL E", 8, &CPU::ExecuteCB13) },
		{ 0x14, CPUInstruction("RL H", 8, &CPU::ExecuteCB14) },
		{ 0x15, CPUInstruction("RL L", 8, &CPU::ExecuteCB15) },
		{ 0x16, CPUInstruction("RL (HL)", 16, &CPU::ExecuteCB16) },
		{ 0x17, CPUInstruction("RL A", 8, &CPU::ExecuteCB17) },
		{ 0x18, CPUInstruction("RR B", 8, &CPU::ExecuteCB18) },
		{ 0x19, CPUInstruction("RR C", 8, &CPU::ExecuteCB19) },
		{ 0x1A, CPUInstruction("RR D", 8, &CPU::ExecuteCB1A) },
		{ 0x1B, CPUInstruction("RR E", 8, &CPU::ExecuteCB1B) },
		{ 0x1C, CPUInstruction("RR H", 8, &CPU::ExecuteCB1C) },
		{ 0x1D, CPUInstruction("RR L", 8, &CPU::ExecuteCB1D) },
		{ 0x1E, CPUInstruction("RR (HL)", 16, &CPU::ExecuteCB1E) },
		{ 0x1F, CPUInstruction("RR A", 8, &CPU::ExecuteCB1F) },
		{ 0x20, CPUInstruction("SLA B", 8, &CPU::ExecuteCB20) },
		{ 0x21, CPUInstruction("SLA C", 8, &CPU::ExecuteCB21) },
		{ 0x22, CPUInstruction("SLA D", 8, &CPU::ExecuteCB22) },
		{ 0x23, CPUInstruction("SLA E", 8, &CPU::ExecuteCB23) },
		{ 0x24, CPUInstruction("SLA H", 8, &CPU::ExecuteCB24) },
		{ 0x25, CPUInstruction("SLA L", 8, &CPU::ExecuteCB25) },
		{ 0x26, CPUInstruction("SLA (HL)", 16, &CPU::ExecuteCB26) },
		{ 0x27, CPUInstruction("SLA A", 8, &CPU::ExecuteCB27) },
		{ 0x28, CPUInstruction("SRA B", 8, &CPU::ExecuteCB28) },
		{ 0x29, CPUInstruction("SRA C", 8, &CPU::ExecuteCB29) },
		{ 0x2A, CPUInstruction("SRA D", 8, &CPU::ExecuteCB2A) },
		{ 0x2B, CPUInstruction("SRA E", 8, &CPU::ExecuteCB2B) },
		{ 0x2C, CPUInstruction("SRA H", 8, &CPU::ExecuteCB2C) },
		{ 0x2D, CPUInstruction("SRA L", 8, &CPU::ExecuteCB2D) },
		{ 0x2E, CPUInstruction("SRA (HL)", 16, &CPU::ExecuteCB2E) },
		{ 0x2F, CPUInstruction("SRA A", 8, &CPU::ExecuteCB2F) },
		{ 0x30, CPUInstruction("SWAP B", 8, &CPU::ExecuteCB30) },
		{ 0x31, CPUInstruction("SWAP C", 8, &CPU::ExecuteCB31) },
		{ 0x32, CPUInstruction("SWAP D", 8, &CPU::ExecuteCB32) },
		{ 0x33, CPUInstruction("SWAP E", 8, &CPU::ExecuteCB33) },
		{ 0x34, CPUInstruction("SWAP H", 8, &CPU::ExecuteCB34) },
		{ 0x35, CPUInstruction("SWAP L", 8, &CPU::ExecuteCB35) },
		{ 0x36, CPUInstruction("SWAP (HL)", 16, &CPU::ExecuteCB36) },
		{ 0x37, CPUInstruction("SWAP A", 8, &CPU::ExecuteCB37) },
		{ 0x38, CPUInstruction("SRL B", 8, &CPU::ExecuteCB38) },
		{ 0x39, CPUInstruction("SRL C", 8, &CPU::ExecuteCB39) },
		{ 0x3A, CPUInstruction("SRL D", 8, &CPU::ExecuteCB3A) },
		{ 0x3B, CPUInstruction("SRL E", 8, &CPU::ExecuteCB3B) },
		{ 0x3C, CPUInstruction("SRL H", 8, &CPU::ExecuteCB3C) },
		{ 0x3D, CPUInstruction("SRL L", 8, &CPU::ExecuteCB3D) },
		{ 0x3E, CPUInstruction("SRL (HL)", 16, &CPU::ExecuteCB3E) },
		{ 0x3F, CPUInstruction("SRL A", 8, &CPU::ExecuteCB3F) },
		{ 0x40, CPUInstruction("BIT 0, B", 8, &CPU::ExecuteCB40) },
		{ 0x41, CPUInstruction("BIT 0, C", 8, &CPU::ExecuteCB41) },
		{ 0x42, CPUInstruction("BIT 0, D", 8, &CPU::ExecuteCB42) },
		{ 0x43, CPUInstruction("BIT 0, E", 8, &CPU::ExecuteCB43) },
		{ 0x44, CPUInstruction("BIT 0, H", 8, &CPU::ExecuteCB44) },
		{ 0x45, CPUInstruction("BIT 0, L", 8, &CPU::ExecuteCB45) },
		{ 0x46, CPUInstruction("BIT 0, (HL)", 12, &CPU::ExecuteCB46) },
		{ 0x47, CPUInstruction("BIT 0, A", 8, &CPU::ExecuteCB47) },
		{ 0x48, CPUInstruction("BIT 1, B", 8, &CPU::ExecuteCB48) },
		{ 0x49, CPUInstruction("BIT 1, C", 8, &CPU::ExecuteCB49) },
		{ 0x4A, CPUInstruction("BIT 1, D", 8, &CPU::ExecuteCB4A) },
		{ 0x4B, CPUInstruction("BIT 1, E", 8, &CPU::ExecuteCB4B) },
		{ 0x4C, CPUInstruction("BIT 1, H", 8, &CPU::ExecuteCB4C) },
		{ 0x4D, CPUInstruction("BIT 1, L", 8, &CPU::ExecuteCB4D) },
		{ 0x4E, CPUInstruction("BIT 1, (HL)", 12, &CPU::ExecuteCB4E) },
		{ 0x4F, CPUInstruction("BIT 1, A", 8, &CPU::ExecuteCB4F) },
		{ 0x50, CPUInstruction("BIT 2, B", 8, &CPU::ExecuteCB50) },
		{ 0x51, CPUInstruction("BIT 2, C", 8, &CPU::ExecuteCB51) },
		{ 0x52, CPUInstruction("BIT 2, D", 8, &CPU::ExecuteCB52) },
		{ 0x53, CPUInstruction("BIT 2, E", 8, &CPU::ExecuteCB53) },
		{ 0x54, CPUInstruction("BIT 2, H", 8, &CPU::ExecuteCB54) },
		{ 0x55, CPUInstruction("BIT 2, L", 8, &CPU::ExecuteCB55) },
		{ 0x56, CPUInstruction("BIT 2, (HL)", 12, &CPU::ExecuteCB56) },
		{ 0x57, CPUInstruction("BIT 2, A", 8, &CPU::ExecuteCB57) },
		{ 0x58, CPUInstruction("BIT 3, B", 8, &CPU::ExecuteCB58) },
		{ 0x59, CPUInstruction("BIT 3, C", 8, &CPU::ExecuteCB59) },
		{ 0x5A, CPUInstruction("BIT 3, D", 8, &CPU::ExecuteCB5A) },
		{ 0x5B, CPUInstruction("BIT 3, E", 8, &CPU::ExecuteCB5B) },
		{ 0x5C, CPUInstruction("BIT 3, H", 8, &CPU::ExecuteCB5C) },
		{ 0x5D, CPUInstruction("BIT 3, L", 8, &CPU::ExecuteCB5D) },
		{ 0x5E, CPUInstruction("BIT 3, (HL)", 12, &CPU::ExecuteCB5E) },
		{ 0x5F, CPUInstruction("BIT 3, A", 8, &CPU::ExecuteCB5F) },
		{ 0x60, CPUInstruction("BIT 4, B", 8, &CPU::ExecuteCB60) },
		{ 0x61, CPUInstruction("BIT 4, C", 8, &CPU::ExecuteCB61) },
		{ 0x62, CPUInstruction("BIT 4, D", 8, &CPU::ExecuteCB62) },
		{ 0x63, CPUInstruction("BIT 4, E", 8, &CPU::ExecuteCB63) },
		{ 0x64, CPUInstruction("BIT 4, H", 8, &CPU::ExecuteCB64) },
		{ 0x65, CPUInstruction("BIT 4, L", 8, &CPU::ExecuteCB65) },
		{ 0x66, CPUInstruction("BIT 4, (HL)", 12, &CPU::ExecuteCB66) },
		{ 0x67, CPUInstruction("BIT 4, A", 8, &CPU::ExecuteCB67) },
		{ 0x68, CPUInstruction("BIT 5, B", 8, &CPU::ExecuteCB68) },
		{ 0x69, CPUInstruction("BIT 5, C", 8, &CPU::ExecuteCB69) },
		{ 0x6A, CPUInstruction("BIT 5, D", 8, &CPU::ExecuteCB6A) },
		{ 0x6B, CPUInstruction("BIT 5, E", 8, &CPU::ExecuteCB6B) },
		{ 0x6C, CPUInstruction("BIT 5, H", 8, &CPU::ExecuteCB6C) },
		{ 0x6D, CPUInstruction("BIT 5, L", 8, &CPU::ExecuteCB6D) },
		{ 0x6E, CPUInstruction("BIT 5, (HL)", 12, &CPU::ExecuteCB6E) },
		{ 0x6F, CPUInstruction("BIT 5, A", 8, &CPU::ExecuteCB6F) },
		{ 0x70, CPUInstruction("BIT 6, B", 8, &CPU::ExecuteCB70) },
		{ 0x71, CPUInstruction("BIT 6, C", 8, &CPU::ExecuteCB71) },
		{ 0x72, CPUInstruction("BIT 6, D", 8, &CPU::ExecuteCB72) },
		{ 0x73, CPUInstruction("BIT 6, E", 8, &CPU::ExecuteCB73) },
		{ 0x74, CPUInstruction("BIT 6, H", 8, &CPU::ExecuteCB74) },
		{ 0x75, CPUInstruction("BIT 6, L", 8, &CPU::ExecuteCB75) },
		{ 0x76, CPUInstruction("BIT 6, (HL)", 12, &CPU::ExecuteCB76) },
		{ 0x77, CPUInstruction("BIT 6, A", 8, &CPU::ExecuteCB77) },
		{ 0x78, CPUInstruction("BIT 7, B", 8, &CPU::ExecuteCB78) },
		{ 0x79, CPUInstruction("BIT 7, C", 8, &CPU::ExecuteCB79) },
		{ 0x7A, CPUInstruction("BIT 7, D", 8, &CPU::ExecuteCB7A) },
		{ 0x7B, CPUInstruction("BIT 7, E", 8, &CPU::ExecuteCB7B) },
		{ 0x7C, CPUInstruction("BIT 7, H", 8, &CPU::ExecuteCB7C) },
		{ 0x7D, CPUInstruction("BIT 7, L", 8, &CPU::ExecuteCB7D) },
		{ 0x7E, CPUInstruction("BIT 7, (HL)", 12, &CPU::ExecuteCB7E) },
		{ 0x7F, CPUInstruction("BIT 7, A", 8, &CPU::ExecuteCB7F) },
		{ 0x80, CPUInstruction("RES 0, B", 8, &CPU::ExecuteCB80) },
		{ 0x81, CPUInstruction("RES 0, C", 8, &CPU::ExecuteCB81) },
		{ 0x82, CPUInstruction("RES 0, D", 8, &CPU::ExecuteCB82) },
		{ 0x83, CPUInstruction("RES 0, E", 8, &CPU::ExecuteCB83) },
		{ 0x84, CPUInstruction("RES 0, H", 8, &CPU::ExecuteCB84) },
		{ 0x85, CPUInstruction("RES 0, L", 8, &CPU::ExecuteCB85) },
		{ 0x86, CPUInstruction("RES 0, (HL)", 16, &CPU::ExecuteCB86) },
		{ 0x87, CPUInstruction("RES 0, A", 8, &CPU::ExecuteCB87) },
		{ 0x88, CPUInstruction("RES 1, B", 8, &CPU::ExecuteCB88) },
		{ 0x89, CPUInstruction("RES 1, C", 8, &CPU::ExecuteCB89) },
		{ 0x8A, CPUInstruction("RES 1, D", 8, &CPU::ExecuteCB8A) },
		{ 0x8B, CPUInstruction("RES 1, E", 8, &CPU::ExecuteCB8B) },
		{ 0x8C, CPUInstruction("RES 1, H", 8, &CPU::ExecuteCB8C) },
		{ 0x8D, CPUInstruction("RES 1, L", 8, &CPU::ExecuteCB8D) },
		{ 0x8E, CPUInstruction("RES 1, (HL)", 16, &CPU::ExecuteCB8E) },
		{ 0x8F, CPUInstruction("RES 1, A", 8, &CPU::ExecuteCB8F) },
		{ 0x90, CPUInstruction("RES 2, B", 8, &CPU::ExecuteCB90) },
		{ 0x91, CPUInstruction("RES 2, C", 8, &CPU::ExecuteCB91) },
		{ 0x92, CPUInstruction("RES 2, D", 8, &CPU::ExecuteCB92) },
		{ 0x93, CPUInstruction("RES 2, E", 8, &CPU::ExecuteCB93) },
		{ 0x94, CPUInstruction("RES 2, H", 8, &CPU::ExecuteCB94) },
		{ 0x95, CPUInstruction("RES 2, L", 8, &CPU::ExecuteCB95) },
		{ 0x96, CPUInstruction("RES 2, (HL)", 16, &CPU::ExecuteCB96) },
		{ 0x97, CPUInstruction("RES 2, A", 8, &CPU::ExecuteCB97) },
		{ 0x98, CPUInstruction("RES 3, B", 8, &CPU::ExecuteCB98) },
		{ 0x99, CPUInstruction("RES 3, C", 8, &CPU::ExecuteCB99) },
		{ 0x9A, CPUInstruction("RES 3, D", 8, &CPU::ExecuteCB9A) },
		{ 0x9B, CPUInstruction("RES 3, E", 8, &CPU::ExecuteCB9B) },
		{ 0x9C, CPUInstruction("RES 3, H", 8, &CPU::ExecuteCB9C) },
		{ 0x9D, CPUInstruction("RES 3, L", 8, &CPU::ExecuteCB9D) },
		{ 0x9E, CPUInstruction("RES 3, (HL)", 16, &CPU::ExecuteCB9E) },
		{ 0x9F, CPUInstruction("RES 3, A", 8, &CPU::ExecuteCB9F) },
		{ 0xA0, CPUInstruction("RES 4, B", 8, &CPU::ExecuteCBA0) },
		{ 0xA1, CPUInstruction("RES 4, C", 8, &CPU::ExecuteCBA1) },
		{ 0xA2, CPUInstruction("RES 4, D", 8, &CPU::ExecuteCBA2) },
		{ 0xA3, CPUInstruction("RES 4, E", 8, &CPU::ExecuteCBA3) },
		{ 0xA4, CPUInstruction("RES 4, H", 8, &CPU::ExecuteCBA4) },
		{ 0xA5, CPUInstruction("RES 4, L", 8, &CPU::ExecuteCBA5) },
		{ 0xA6, CPUInstruction("RES 4, (HL)", 16, &CPU::ExecuteCBA6) },
		{ 0xA7, CPUInstruction("RES 4, A", 8, &CPU::ExecuteCBA7) },
		{ 0xA8, CPUInstruction("RES 5, B", 8, &CPU::ExecuteCBA8) },
		{ 0xA9, CPUInstruction("RES 5, C", 8, &CPU::ExecuteCBA9) },
		{ 0xAA, CPUInstruction("RES 5, D", 8, &CPU::ExecuteCBAA) },
		{ 0xAB, CPUInstruction("RES 5, E", 8, &CPU::ExecuteCBAB) },
		{ 0xAC, CPUInstruction("RES 5, H", 8, &CPU::ExecuteCBAC) },
		{ 0xAD, CPUInstruction("RES 5, L", 8, &CPU::ExecuteCBAD) },
		{ 0xAE, CPUInstruction("RES 5, (HL)", 16, &CPU::ExecuteCBAE) },
		{ 0xAF, CPUInstruction("RES 5, A", 8, &CPU::ExecuteCBAF) },
		{ 0xB0, CPUInstruction("RES 6, B", 8, &CPU::ExecuteCBB0) },
		{ 0xB1, CPUInstruction("RES 6, C", 8, &CPU::ExecuteCBB1) },
		{ 0xB2, CPUInstruction("RES 6, D", 8, &CPU::ExecuteCBB2) },
		{ 0xB3, CPUInstruction("RES 6, E", 8, &CPU::ExecuteCBB3) },
		{ 0xB4, CPUInstruction("RES 6, H", 8, &CPU::ExecuteCBB4) },
		{ 0xB5, CPUInstruction("RES 6, L", 8, &CPU::ExecuteCBB5) },
		{ 0xB6, CPUInstruction("RES 6, (HL)", 16, &CPU::ExecuteCBB6) },
		{ 0xB7, CPUInstruction("RES 6, A", 8, &CPU::ExecuteCBB7) },
		{ 0xB8, CPUInstruction("RES 7, B", 8, &CPU::ExecuteCBB8) },
		{ 0xB9, CPUInstruction("RES 7, C", 8, &CPU::ExecuteCBB9) },
		{ 0xBA, CPUInstruction("RES 7, D", 8, &CPU::ExecuteCBBA) },
		{ 0xBB, CPUInstruction("RES 7, E", 8, &CPU::ExecuteCBBB) },
		{ 0xBC, CPUInstruction("RES 7, H", 8, &CPU::ExecuteCBBC) },
		{ 0xBD, CPUInstruction("RES 7, L", 8, &CPU::ExecuteCBBD) },
		{ 0xBE, CPUInstruction("RES 7, (HL)", 16, &CPU::ExecuteCBBE) },
		{ 0xBF, CPUInstruction("RES 7, A", 8, &CPU::ExecuteCBBF) },
		{ 0xC0, CPUInstruction("SET 0, B", 8, &CPU::ExecuteCBC0) },
		{ 0xC1, CPUInstruction("SET 0, C", 8, &CPU::ExecuteCBC1) },
		{ 0xC2, CPUInstruction("SET 0, D", 8, &CPU::ExecuteCBC2) },
		{ 0xC3, CPUInstruction("SET 0, E", 8, &CPU::ExecuteCBC3) },
		{ 0xC4, CPUInstruction("SET 0, H", 8, &CPU::ExecuteCBC4) },
		{ 0xC5, CPUInstruction("SET 0, L", 8, &CPU::ExecuteCBC5) },
		{ 0xC6, CPUInstruction("SET 0, (HL)", 16, &CPU::ExecuteCBC6) },
		{ 0xC7, CPUInstruction("SET 0, A", 8, &CPU::ExecuteCBC7) },
		{ 0xC8, CPUInstruction("SET 1, B", 8, &CPU::ExecuteCBC8) },
		{ 0xC9, CPUInstruction("SET 1, C", 8, &CPU::ExecuteCBC9) },
		{ 0xCA, CPUInstruction("SET 1, D", 8, &CPU::ExecuteCBCA) },
		{ 0xCB, CPUInstruction("SET 1, E", 8, &CPU::ExecuteCBCB) },
		{ 0xCC, CPUInstruction("SET 1, H", 8, &CPU::ExecuteCBCC) },
		{ 0xCD, CPUInstruction("SET 1, L", 8, &CPU::ExecuteCBCD) },
		{ 0xCE, CPUInstruction("SET 1, (HL)", 16, &CPU::ExecuteCBCE) },
		{ 0xCF, CPUInstruction("SET 1, A", 8, &CPU::ExecuteCBCF) },
		{ 0xD0, CPUInstruction("SET 2, B", 8, &CPU::ExecuteCBD0) },
		{ 0xD1, CPUInstruction("SET 2, C", 8, &CPU::ExecuteCBD1) },
		{ 0xD2, CPUInstruction("SET 2, D", 8, &CPU::ExecuteCBD2) },
		{ 0xD3, CPUInstruction("SET 2, E", 8, &CPU::ExecuteCBD3) },
		{ 0xD4, CPUInstruction("SET 2, H", 8, &CPU::ExecuteCBD4) },
		{ 0xD5, CPUInstruction("SET 2, L", 8, &CPU::ExecuteCBD5) },
		{ 0xD6, CPUInstruction("SET 2, (HL)", 16, &CPU::ExecuteCBD6) },
		{ 0xD7, CPUInstruction("SET 2, A", 8, &CPU::ExecuteCBD7) },
		{ 0xD8, CPUInstruction("SET 3, B", 8, &CPU::ExecuteCBD8) },
		{ 0xD9, CPUInstruction("SET 3, C", 8, &CPU::ExecuteCBD9) },
		{ 0xDA, CPUInstruction("SET 3, D", 8, &CPU::ExecuteCBDA) },
		{ 0xDB, CPUInstruction("SET 3, E", 8, &CPU::ExecuteCBDB) },
		{ 0xDC, CPUInstruction("SET 3, H", 8, &CPU::ExecuteCBDC) },
		{ 0xDD, CPUInstruction("SET 3, L", 8, &CPU::ExecuteCBDD) },
		{ 0xDE, CPUInstruction("SET 3, (HL)", 16, &CPU::ExecuteCBDE) },
		{ 0xDF, CPUInstruction("SET 3, A", 8, &CPU::ExecuteCBDF) },
		{ 0xE0, CPUInstruction("SET 4, B", 8, &CPU::ExecuteCBE0) },
		{ 0xE1, CPUInstruction("SET 4, C", 8, &CPU::ExecuteCBE1) },
		{ 0xE2, CPUInstruction("SET 4, D", 8, &CPU::ExecuteCBE2) },
		{ 0xE3, CPUInstruction("SET 4, E", 8, &CPU::ExecuteCBE3) },
		{ 0xE4, CPUInstruction("SET 4, H", 8, &CPU::ExecuteCBE4) },
		{ 0xE5, CPUInstruction("SET 4, L", 8, &CPU::ExecuteCBE5) },
		{ 0xE6, CPUInstruction("SET 4, (HL)", 16, &CPU::ExecuteCBE6) },
		{ 0xE7, CPUInstruction("SET 4, A", 8, &CPU::ExecuteCBE7) },
		{ 0xE8, CPUInstruction("SET 5, B", 8, &CPU::ExecuteCBE8) },
		{ 0xE9, CPUInstruction("SET 5, C", 8, &CPU::ExecuteCBE9) },
		{ 0xEA, CPUInstruction("SET 5, D", 8, &CPU::ExecuteCBEA) },
		{ 0xEB, CPUInstruction("SET 5, E", 8, &CPU::ExecuteCBEB) },
		{ 0xEC, CPUInstruction("SET 5, H", 8, &CPU::ExecuteCBEC) },
		{ 0xED, CPUInstruction("SET 5, L", 8, &CPU::ExecuteCBED) },
		{ 0xEE, CPUInstruction("SET 5, (HL)", 16, &CPU::ExecuteCBEE) },
		{ 0xEF, CPUInstruction("SET 5, A", 8, &CPU::ExecuteCBEF) },
		{ 0xF0, CPUInstruction("SET 6, B", 8, &CPU::ExecuteCBF0) },
		{ 0xF1, CPUInstruction("SET 6, C", 8, &CPU::ExecuteCBF1) },
		{ 0xF2, CPUInstruction("SET 6, D", 8, &CPU::ExecuteCBF2) },
		{ 0xF3, CPUInstruction("SET 6, E", 8, &CPU::ExecuteCBF3) },
		{ 0xF4, CPUInstruction("SET 6, H", 8, &CPU::ExecuteCBF4) },
		{ 0xF5, CPUInstruction("SET 6, L", 8, &CPU::ExecuteCBF5) },
		{ 0xF6, CPUInstruction("SET 6, (HL)", 16, &CPU::ExecuteCBF6) },
		{ 0xF7, CPUInstruction("SET 6, A", 8, &CPU::ExecuteCBF7) },
		{ 0xF8, CPUInstruction("SET 7, B", 8, &CPU::ExecuteCBF8) },
		{ 0xF9, CPUInstruction("SET 7, C", 8, &CPU::ExecuteCBF9) },
		{ 0xFA, CPUInstruction("SET 7, D", 8, &CPU::ExecuteCBFA) },
		{ 0xFB, CPUInstruction("SET 7, E", 8, &CPU::ExecuteCBFB) },
		{ 0xFC, CPUInstruction("SET 7, H", 8, &CPU::ExecuteCBFC) },
		{ 0xFD, CPUInstruction("SET 7, L", 8, &CPU::ExecuteCBFD) },
		{ 0xFE, CPUInstruction("SET 7, (HL)", 16, &CPU::ExecuteCBFE) },
		{ 0xFF, CPUInstruction("SET 7, A", 8, &CPU::ExecuteCBFF) }
	};



	void CPU::NOP()
	{
		// Do nothing
	}

	void CPU::LD_RR_U16(Register16& reg)
	{
		reg.SetData(Fetch16());
	}

	void CPU::LD_ADDR_RR_R(Register16& addressReg, Register8& sourceReg)
	{
		memoryManagementUnit.Write(addressReg.GetData(), sourceReg.GetData());
	}

	void CPU::LD_R_U8(Register8& reg)
	{
		reg.SetData(Fetch8());
	}

	void CPU::LD_ADDR_U16_RR(Register16& sourceReg)
	{
		Set16BitDataInMemory(Fetch16(), sourceReg.GetData());
	}

	void CPU::INC_RR(Register16& reg)
	{
		reg.Increment();
	}

	void CPU::INC_R(Register8& reg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), 1 }));

		reg.Increment();
		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(false);
	}

	void CPU::DEC_R(Register8& reg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), 1 }, true));

		reg.Decrement();
		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(true);
	}

	void CPU::RLC_R(Register8& reg)
	{
		uint8_t carry = reg.GetData() >> 7;

		uint8_t result = (reg.GetData() << 1) | carry;
		reg.SetData(result);

		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::RLCA()
	{
		Register8& reg = regAF.GetHighRegister();
		uint8_t carry = reg.GetData() >> 7;
		uint8_t result = (reg.GetData() << 1) | carry;

		reg.SetData(result);
		ChangeZeroFlag(false);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::RLC_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t byteAtAddress = memoryManagementUnit.Read(address);
		uint8_t carry = byteAtAddress >> 7;
		uint8_t result = (byteAtAddress << 1) | carry;

		memoryManagementUnit.Write(address, result);
		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::ADD_RR_RR(Register16& destinationReg, Register16& sourceReg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is16BitHalfCarryRequired({ destinationReg.GetData(), sourceReg.GetData() }));

		int result = destinationReg.GetData() + sourceReg.GetData();
		destinationReg.SetData(result);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is16BitOverflow(result));
	}

	void CPU::LD_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		destinationReg.SetData(memoryManagementUnit.Read(addressReg.GetData()));
	}

	void CPU::DEC_RR(Register16& reg)
	{
		reg.Decrement();
	}

	void CPU::RRC_R(Register8& reg)
	{
		ChangeCarryFlag(reg.GetBit(0));

		uint8_t result = (reg.GetData() >> 1) | (reg.GetBit(0) << 7);
		reg.SetData(result);
		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
	}

	void CPU::RRCA()
	{
		Register8& reg = regAF.GetHighRegister();

		ChangeCarryFlag(reg.GetBit(0));

		uint8_t result = (reg.GetData() >> 1) | (reg.GetBit(0) << 7);
		reg.SetData(result);
		ChangeZeroFlag(false);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
	}

	void CPU::RRC_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);

		uint8_t result = (value >> 1) | (value << 7);
		memoryManagementUnit.Write(address, result);
		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(value & 1);
	}

	void CPU::STOP()
	{
		// TODO: Implement
	}

	void CPU::RL_R(Register8& reg)
	{
		uint8_t previousCarry = GetCarryFlag();
		uint8_t currentCarry = reg.GetBit(7);

		reg.SetData((reg.GetData() << 1) | previousCarry);

		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(currentCarry);
	}

	void CPU::RLA()
	{
		Register8& reg = regAF.GetHighRegister();
		uint8_t regData = reg.GetData();

		uint8_t previousCarry = GetCarryFlag();
		uint8_t currentCarry = reg.GetBit(7);;
		reg.SetData((regData << 1) | previousCarry);

		ChangeZeroFlag(false);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(currentCarry);
	}

	void CPU::RL_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);

		uint8_t previousCarry = GetCarryFlag();
		uint8_t currentCarry = value >> 7;

		uint8_t result = (value << 1) | previousCarry;
		memoryManagementUnit.Write(address, result);

		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(currentCarry);
	}

	void CPU::JR_I8(bool areFlagsSet)
	{
		// Even if a jump is not executed, a fetch should still
		// be performed to ensure the program counter is incremented properly.
		int8_t fetchedData = Fetch8();
		if (areFlagsSet)
		{
			programCounter.SetData(programCounter.GetData() + fetchedData);
			currentCycles = 12;
		}
		else currentCycles = 8;
	}

	void CPU::RR_R(Register8& reg)
	{
		uint8_t previousCarry = GetCarryFlag();
		uint8_t currentCarry = reg.GetBit(0);

		reg.SetData((reg.GetData() >> 1) | (previousCarry << 7));

		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(currentCarry);
	}

	void CPU::RRA()
	{
		Register8& reg = regAF.GetHighRegister();

		uint8_t previousCarry = GetCarryFlag();
		uint8_t currentCarry = reg.GetBit(0);

		uint8_t regData = reg.GetData();
		reg.SetData((regData >> 1) | (previousCarry << 7));

		ChangeZeroFlag(false);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(currentCarry);
	}

	void CPU::RR_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);

		uint8_t previousCarry = GetCarryFlag();
		uint8_t currentCarry = value & 1;

		uint8_t result = (value >> 1) | (previousCarry << 7);
		memoryManagementUnit.Write(address, result);

		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(currentCarry);
	}

	void CPU::LD_ADDR_RR_INC_R(Register16& addressReg, Register8& sourceReg)
	{
		memoryManagementUnit.Write(addressReg.GetData(), sourceReg.GetData());
		addressReg.Increment();
	}

	void CPU::DAA()
	{
		uint8_t result = regAF.GetHighByte();
		bool carry = false;

		if (GetSubtractionFlag())
		{
			if (GetCarryFlag())
			{
				result -= 0x60;
				carry = true;
			}
			if (GetHalfCarryFlag()) result -= 0x06;
		}
		else
		{
			if (GetCarryFlag() || (result > 0x99))
			{
				result += 0x60;
				carry = true;
			}
			if (GetHalfCarryFlag() || ((result & 0x0F) > 0x09)) result += 0x06;
		}

		regAF.SetHighByte(result);
		ChangeZeroFlag(result == 0);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::LD_R_ADDR_RR_INC(Register8& destinationReg, Register16& addressReg)
	{
		destinationReg.SetData(memoryManagementUnit.Read(addressReg.GetData()));
		addressReg.Increment();
	}

	void CPU::CPL()
	{
		regAF.SetHighByte(regAF.GetHighByte() ^ 0xFF);
		ChangeSubtractionFlag(true);
		ChangeHalfCarryFlag(true);
	}

	void CPU::LD_ADDR_RR_DEC_R(Register16& addressReg, Register8& sourceReg)
	{
		memoryManagementUnit.Write(addressReg.GetData(), sourceReg.GetData());
		addressReg.Decrement();
	}

	void CPU::INC_ADDR_RR(Register16& reg)
	{
		uint8_t operand = memoryManagementUnit.Read(reg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ operand, 1 }));

		operand++;
		memoryManagementUnit.Write(reg.GetData(), operand);

		ChangeZeroFlag(operand == 0);
		ChangeSubtractionFlag(false);
	}

	void CPU::DEC_ADDR_RR(Register16& reg)
	{
		uint8_t operand = memoryManagementUnit.Read(reg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ operand, 1 }, true));

		operand--;
		memoryManagementUnit.Write(reg.GetData(), operand);

		ChangeZeroFlag(operand == 0);
		ChangeSubtractionFlag(true);
	}

	void CPU::LD_ADDR_RR_U8(Register16& addressReg)
	{
		memoryManagementUnit.Write(addressReg.GetData(), Fetch8());
	}

	void CPU::SCF()
	{
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(true);
	}

	void CPU::LD_R_ADDR_RR_DEC(Register8& destinationReg, Register16& addressReg)
	{
		destinationReg.SetData(memoryManagementUnit.Read(addressReg.GetData()));
		addressReg.Decrement();
	}

	void CPU::CCF()
	{
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(GetCarryFlag() ^ 1);
	}

	void CPU::LD_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		destinationReg.SetData(sourceReg.GetData());
	}

	void CPU::HALT()
	{
		// Halt until interrupt occurs
		isHalted = true;
	}

	void CPU::ADD_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), sourceReg.GetData() }));

		int result = destinationReg.GetData() + sourceReg.GetData();
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::ADD_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		uint8_t byteAtAddress = memoryManagementUnit.Read(addressReg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), byteAtAddress }));

		int result = destinationReg.GetData() + byteAtAddress;
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::ADC_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), sourceReg.GetData(), GetCarryFlag() }));

		int result = destinationReg.GetData() + sourceReg.GetData() + GetCarryFlag();
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::ADC_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		uint8_t byteAtAddress = memoryManagementUnit.Read(addressReg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), byteAtAddress, GetCarryFlag() }));

		int result = destinationReg.GetData() + byteAtAddress + GetCarryFlag();
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SUB_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), sourceReg.GetData() }, true));

		int result = destinationReg.GetData() - sourceReg.GetData();
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SUB_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		uint8_t byteAtAddress = memoryManagementUnit.Read(addressReg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), byteAtAddress }, true));

		int result = destinationReg.GetData() - byteAtAddress;
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SBC_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), sourceReg.GetData(), GetCarryFlag() }, true));

		int result = destinationReg.GetData() - sourceReg.GetData() - GetCarryFlag();
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SBC_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		uint8_t byteAtAddress = memoryManagementUnit.Read(addressReg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), byteAtAddress, GetCarryFlag() }, true));

		int result = destinationReg.GetData() - byteAtAddress - GetCarryFlag();
		destinationReg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::AND_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		destinationReg.SetData(destinationReg.GetData() & sourceReg.GetData());
		ChangeZeroFlag(destinationReg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(true);
		ChangeCarryFlag(false);
	}

	void CPU::AND_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		destinationReg.SetData(destinationReg.GetData() & memoryManagementUnit.Read(addressReg.GetData()));
		ChangeZeroFlag(destinationReg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(true);
		ChangeCarryFlag(false);
	}

	void CPU::XOR_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		destinationReg.SetData(destinationReg.GetData() ^ sourceReg.GetData());
		ChangeZeroFlag(destinationReg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::XOR_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		destinationReg.SetData(destinationReg.GetData() ^ memoryManagementUnit.Read(addressReg.GetData()));
		ChangeZeroFlag(destinationReg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::OR_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		destinationReg.SetData(destinationReg.GetData() | sourceReg.GetData());
		ChangeZeroFlag(destinationReg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::OR_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		destinationReg.SetData(destinationReg.GetData() | memoryManagementUnit.Read(addressReg.GetData()));
		ChangeZeroFlag(destinationReg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::CP_R_R(Register8& destinationReg, Register8& sourceReg)
	{
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), sourceReg.GetData() }, true));

		int result = destinationReg.GetData() - sourceReg.GetData();
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::CP_R_ADDR_RR(Register8& destinationReg, Register16& addressReg)
	{
		uint8_t byteAtAddress = memoryManagementUnit.Read(addressReg.GetData());
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ destinationReg.GetData(), byteAtAddress }, true));

		int result = destinationReg.GetData() - byteAtAddress;
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::RET(bool areFlagsSet)
	{
		if (areFlagsSet)
		{
			RET();
			currentCycles = 20;
		}
		else currentCycles = 8;
	}

	void CPU::RET()
	{
		programCounter.SetData(Get16BitDataFromMemory(stackPointer.GetData()));
		stackPointer.Increase(2);
	}

	void CPU::POP_RR(Register16& reg)
	{
		reg.SetData(Get16BitDataFromMemory(stackPointer.GetData()));
		stackPointer.Increase(2);
	}

	void CPU::POP_AF()
	{
		// The lower nibble of register F cannot be changed
		regAF.SetData(Get16BitDataFromMemory(stackPointer.GetData()) & 0xFFF0);
		stackPointer.Increase(2);
	}

	void CPU::JP_U16(bool areFlagsSet)
	{
		uint16_t fetchedData = Fetch16();
		if (areFlagsSet)
		{
			programCounter.SetData(fetchedData);
			currentCycles = 16;
		}
		else currentCycles = 12;
	}

	void CPU::CALL_U16(bool areFlagsSet)
	{
		uint16_t fetchedData = Fetch16();
		if (areFlagsSet)
		{
			CALL(fetchedData);
			currentCycles = 24;
		}
		else currentCycles = 12;
	}

	void CPU::CALL(uint16_t address)
	{
		stackPointer.Decrease(2);
		Set16BitDataInMemory(stackPointer.GetData(), programCounter.GetData());
		programCounter.SetData(address);
	}

	void CPU::PUSH_RR(Register16& reg)
	{
		stackPointer.Decrease(2);
		Set16BitDataInMemory(stackPointer.GetData(), reg.GetData());
	}

	void CPU::ADD_R_U8(Register8& reg)
	{
		uint8_t operand = Fetch8();
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), operand }));

		int result = reg.GetData() + operand;
		reg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::RST(uint16_t address)
	{
		CALL(address);
	}

	void CPU::ADC_R_U8(Register8& reg)
	{
		uint8_t operand = Fetch8();
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), operand, GetCarryFlag() }));

		int result = reg.GetData() + operand + GetCarryFlag();
		reg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SUB_R_U8(Register8& reg)
	{
		uint8_t operand = Fetch8();
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), operand }, true));

		int result = reg.GetData() - operand;
		reg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::RETI()
	{
		RET();
		interruptMasterEnableFlag = true;
	}

	void CPU::SBC_R_U8(Register8& reg)
	{
		uint8_t operand = Fetch8();
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), operand, GetCarryFlag() }, true));

		int result = reg.GetData() - operand - GetCarryFlag();
		reg.SetData(result);

		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::LD_ADDR_FF00_U8_R(Register8& reg)
	{
		memoryManagementUnit.Write(0xFF00 + Fetch8(), reg.GetData());
	}

	void CPU::LD_ADDR_FF00_R_R(Register8& addressReg, Register8& sourceReg)
	{
		memoryManagementUnit.Write(0xFF00 + addressReg.GetData(), sourceReg.GetData());
	}

	void CPU::AND_R_U8(Register8& reg)
	{
		reg.SetData(reg.GetData() & Fetch8());
		ChangeZeroFlag(reg.GetData() == 0);

		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(true);
		ChangeCarryFlag(false);
	}

	void CPU::ADD_RR_I8(Register16& destinationReg)
	{
		uint8_t operand = Fetch8();

		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ (uint8_t)destinationReg.GetData(), operand }));
		ChangeCarryFlag(Arithmetic::Is8BitOverflow((destinationReg.GetData() & 0xFF) + operand));

		int result = destinationReg.GetData() + (int8_t)operand;
		destinationReg.SetData(result);

		ChangeZeroFlag(false);
		ChangeSubtractionFlag(false);
	}

	void CPU::JP_RR(Register16& reg, bool areFlagsSet)
	{
		if (areFlagsSet)
		{
			programCounter.SetData(reg.GetData());
			currentCycles = 16;
		}
		else currentCycles = 12;
	}

	void CPU::LD_ADDR_U16_R(Register8& reg)
	{
		memoryManagementUnit.Write(Fetch16(), reg.GetData());
	}

	void CPU::XOR_R_U8(Register8& reg)
	{
		uint8_t operand = Fetch8();
		reg.SetData(reg.GetData() ^ operand);
		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::LD_R_ADDR_FF00_U8(Register8& reg)
	{
		reg.SetData(memoryManagementUnit.Read(0xFF00 + Fetch8()));
	}

	void CPU::LD_R_ADDR_FF00_R(Register8& destinationReg, Register8& addressReg)
	{
		destinationReg.SetData(memoryManagementUnit.Read(0xFF00 + addressReg.GetData()));
	}

	void CPU::DI()
	{
		interruptMasterEnableFlag = false;
	}

	void CPU::OR_R_U8(Register8& reg)
	{
		reg.SetData(reg.GetData() | Fetch8());
		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::LD_RR_RR_I8(Register16& destinationReg, Register16& sourceReg)
	{
		uint8_t operand = Fetch8();

		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ (uint8_t)sourceReg.GetData(), operand }));
		ChangeCarryFlag(Arithmetic::Is8BitOverflow((sourceReg.GetData() & 0xFF) + operand));

		int result = sourceReg.GetData() + (int8_t)operand;
		destinationReg.SetData(result);

		ChangeZeroFlag(false);
		ChangeSubtractionFlag(false);
	}

	void CPU::LD_RR_RR(Register16& destinationReg, Register16& sourceReg)
	{
		destinationReg.SetData(sourceReg.GetData());
	}

	void CPU::LD_R_ADDR_U16(Register8& destinationReg)
	{
		destinationReg.SetData(memoryManagementUnit.Read(Fetch16()));
	}

	void CPU::EI()
	{
		interruptMasterEnableFlag = true;
	}

	void CPU::CP_R_U8(Register8& reg)
	{
		uint8_t operand = Fetch8();
		ChangeHalfCarryFlag(Arithmetic::Is8BitHalfCarryRequired({ reg.GetData(), operand }, true));

		int result = reg.GetData() - operand;
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(true);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SLA_R(Register8& reg)
	{
		int result = reg.GetData() << 1;
		reg.SetData(result);
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SLA_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		int result = memoryManagementUnit.Read(address) << 1;
		memoryManagementUnit.Write(address, result);
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(Arithmetic::Is8BitOverflow(result));
	}

	void CPU::SRA_R(Register8& reg)
	{
		uint8_t carry = reg.GetBit(0);
		// Shift right while retaining the sign bit (most significant bit)
		int result = (reg.GetData() >> 1) | (reg.GetData() & 0b10000000);
		reg.SetData(result);
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::SRA_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);
		// Shift right while retaining the sign bit (most significant bit)
		int result = (value >> 1) | (value & 0b10000000);
		memoryManagementUnit.Write(address, result);
		ChangeZeroFlag(((uint8_t)result) == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(value & 1);
	}

	void CPU::SRL_R(Register8& reg)
	{
		uint8_t carry = reg.GetBit(0);
		uint8_t result = reg.GetData() >> 1;
		reg.SetData(result);
		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::SRL_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t operand = memoryManagementUnit.Read(address);
		uint8_t carry = operand & 1;
		operand >>= 1;
		memoryManagementUnit.Write(address, operand);
		ChangeZeroFlag(operand == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(carry);
	}

	void CPU::SWAP_R(Register8& reg)
	{
		// Swap the low and high nibbles
		reg.SetData(((reg.GetData() & 0x0F) << 4) | ((reg.GetData() & 0xF0) >> 4));
		ChangeZeroFlag(reg.GetData() == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::SWAP_ADDR_RR(Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t byteAtAddress = memoryManagementUnit.Read(address);
		uint8_t result = ((byteAtAddress & 0x0F) << 4) | ((byteAtAddress & 0xF0) >> 4);
		// Swap the low and high nibbles
		memoryManagementUnit.Write(address, result);
		ChangeZeroFlag(result == 0);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(false);
		ChangeCarryFlag(false);
	}

	void CPU::BIT_N_R(uint8_t bitNum, Register8& reg)
	{
		// Clear the zero flag if the bit at 'bitNum' is set, or set the zero flag if the bit at 'bitNum' is not set.
		ChangeZeroFlag((~reg.GetData() >> bitNum) & 1);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(true);
	}

	void CPU::BIT_N_ADDR_RR(uint8_t bitNum, Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);
		// Clear the zero flag if the bit at 'bitNum' is set, or set the zero flag if the bit at 'bitNum' is not set.
		ChangeZeroFlag((~value >> bitNum) & 1);
		ChangeSubtractionFlag(false);
		ChangeHalfCarryFlag(true);
	}

	void CPU::RES_N_R(uint8_t bitNum, Register8& reg)
	{
		reg.SetData((reg.GetData()) & ~(1 << bitNum));
	}

	void CPU::RES_N_ADDR_RR(uint8_t bitNum, Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);
		memoryManagementUnit.Write(address, (value) & ~(1 << bitNum));
	}

	void CPU::SET_N_R(uint8_t bitNum, Register8& reg)
	{
		reg.SetData((reg.GetData()) | (1 << bitNum));
	}

	void CPU::SET_N_ADDR_RR(uint8_t bitNum, Register16& addressReg)
	{
		uint16_t address = addressReg.GetData();
		uint8_t value = memoryManagementUnit.Read(address);
		memoryManagementUnit.Write(address, (value) | (1 << bitNum));
	}

	void CPU::Execute00() { NOP(); }
	void CPU::Execute01() { LD_RR_U16(GetRegisterBC()); }
	void CPU::Execute02() { LD_ADDR_RR_R(GetRegisterBC(), GetRegisterA()); }
	void CPU::Execute03() { INC_RR(GetRegisterBC()); }
	void CPU::Execute04() { INC_R(GetRegisterB()); }
	void CPU::Execute05() { DEC_R(GetRegisterB()); }
	void CPU::Execute06() { LD_R_U8(GetRegisterB()); }
	void CPU::Execute07() { RLCA(); }
	void CPU::Execute08() { LD_ADDR_U16_RR(stackPointer); }
	void CPU::Execute09() { ADD_RR_RR(GetRegisterHL(), GetRegisterBC()); }
	void CPU::Execute0A() { LD_R_ADDR_RR(GetRegisterA(), GetRegisterBC()); }
	void CPU::Execute0B() { DEC_RR(GetRegisterBC()); }
	void CPU::Execute0C() { INC_R(GetRegisterC()); }
	void CPU::Execute0D() { DEC_R(GetRegisterC()); }
	void CPU::Execute0E() { LD_R_U8(GetRegisterC()); }
	void CPU::Execute0F() { RRCA(); }

	void CPU::Execute10() { STOP(); }
	void CPU::Execute11() { LD_RR_U16(GetRegisterDE()); }
	void CPU::Execute12() { LD_ADDR_RR_R(GetRegisterDE(), GetRegisterA()); }
	void CPU::Execute13() { INC_RR(GetRegisterDE()); }
	void CPU::Execute14() { INC_R(GetRegisterD()); }
	void CPU::Execute15() { DEC_R(GetRegisterD()); }
	void CPU::Execute16() { LD_R_U8(GetRegisterD()); }
	void CPU::Execute17() { RLA(); }
	void CPU::Execute18() { JR_I8(); }
	void CPU::Execute19() { ADD_RR_RR(GetRegisterHL(), GetRegisterDE()); }
	void CPU::Execute1A() { LD_R_ADDR_RR(GetRegisterA(), GetRegisterDE()); }
	void CPU::Execute1B() { DEC_RR(GetRegisterDE()); }
	void CPU::Execute1C() { INC_R(GetRegisterE()); }
	void CPU::Execute1D() { DEC_R(GetRegisterE()); }
	void CPU::Execute1E() { LD_R_U8(GetRegisterE()); }
	void CPU::Execute1F() { RRA(); }

	void CPU::Execute20() { JR_I8(!GetZeroFlag()); }
	void CPU::Execute21() { LD_RR_U16(GetRegisterHL()); }
	void CPU::Execute22() { LD_ADDR_RR_INC_R(GetRegisterHL(), GetRegisterA()); }
	void CPU::Execute23() { INC_RR(GetRegisterHL()); }
	void CPU::Execute24() { INC_R(GetRegisterH()); }
	void CPU::Execute25() { DEC_R(GetRegisterH()); }
	void CPU::Execute26() { LD_R_U8(GetRegisterH()); }
	void CPU::Execute27() { DAA(); }
	void CPU::Execute28() { JR_I8(GetZeroFlag()); }
	void CPU::Execute29() { ADD_RR_RR(GetRegisterHL(), GetRegisterHL()); }
	void CPU::Execute2A() { LD_R_ADDR_RR_INC(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute2B() { DEC_RR(GetRegisterHL()); }
	void CPU::Execute2C() { INC_R(GetRegisterL()); }
	void CPU::Execute2D() { DEC_R(GetRegisterL()); }
	void CPU::Execute2E() { LD_R_U8(GetRegisterL()); }
	void CPU::Execute2F() { CPL(); }

	void CPU::Execute30() { JR_I8(!GetCarryFlag()); }
	void CPU::Execute31() { LD_RR_U16(stackPointer); }
	void CPU::Execute32() { LD_ADDR_RR_DEC_R(GetRegisterHL(), GetRegisterA()); }
	void CPU::Execute33() { INC_RR(stackPointer); }
	void CPU::Execute34() { INC_ADDR_RR(GetRegisterHL()); }
	void CPU::Execute35() { DEC_ADDR_RR(GetRegisterHL()); }
	void CPU::Execute36() { LD_ADDR_RR_U8(GetRegisterHL()); }
	void CPU::Execute37() { SCF(); }
	void CPU::Execute38() { JR_I8(GetCarryFlag()); }
	void CPU::Execute39() { ADD_RR_RR(GetRegisterHL(), stackPointer); }
	void CPU::Execute3A() { LD_R_ADDR_RR_DEC(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute3B() { DEC_RR(stackPointer); }
	void CPU::Execute3C() { INC_R(GetRegisterA()); }
	void CPU::Execute3D() { DEC_R(GetRegisterA()); }
	void CPU::Execute3E() { LD_R_U8(GetRegisterA()); }
	void CPU::Execute3F() { CCF(); }

	void CPU::Execute40() { LD_R_R(GetRegisterB(), GetRegisterB()); }
	void CPU::Execute41() { LD_R_R(GetRegisterB(), GetRegisterC()); }
	void CPU::Execute42() { LD_R_R(GetRegisterB(), GetRegisterD()); }
	void CPU::Execute43() { LD_R_R(GetRegisterB(), GetRegisterE()); }
	void CPU::Execute44() { LD_R_R(GetRegisterB(), GetRegisterH()); }
	void CPU::Execute45() { LD_R_R(GetRegisterB(), GetRegisterL()); }
	void CPU::Execute46() { LD_R_ADDR_RR(GetRegisterB(), GetRegisterHL()); }
	void CPU::Execute47() { LD_R_R(GetRegisterB(), GetRegisterA()); }
	void CPU::Execute48() { LD_R_R(GetRegisterC(), GetRegisterB()); }
	void CPU::Execute49() { LD_R_R(GetRegisterC(), GetRegisterC()); }
	void CPU::Execute4A() { LD_R_R(GetRegisterC(), GetRegisterD()); }
	void CPU::Execute4B() { LD_R_R(GetRegisterC(), GetRegisterE()); }
	void CPU::Execute4C() { LD_R_R(GetRegisterC(), GetRegisterH()); }
	void CPU::Execute4D() { LD_R_R(GetRegisterC(), GetRegisterL()); }
	void CPU::Execute4E() { LD_R_ADDR_RR(GetRegisterC(), GetRegisterHL()); }
	void CPU::Execute4F() { LD_R_R(GetRegisterC(), GetRegisterA()); }

	void CPU::Execute50() { LD_R_R(GetRegisterD(), GetRegisterB()); }
	void CPU::Execute51() { LD_R_R(GetRegisterD(), GetRegisterC()); }
	void CPU::Execute52() { LD_R_R(GetRegisterD(), GetRegisterD()); }
	void CPU::Execute53() { LD_R_R(GetRegisterD(), GetRegisterE()); }
	void CPU::Execute54() { LD_R_R(GetRegisterD(), GetRegisterH()); }
	void CPU::Execute55() { LD_R_R(GetRegisterD(), GetRegisterL()); }
	void CPU::Execute56() { LD_R_ADDR_RR(GetRegisterD(), GetRegisterHL()); }
	void CPU::Execute57() { LD_R_R(GetRegisterD(), GetRegisterA()); }
	void CPU::Execute58() { LD_R_R(GetRegisterE(), GetRegisterB()); }
	void CPU::Execute59() { LD_R_R(GetRegisterE(), GetRegisterC()); }
	void CPU::Execute5A() { LD_R_R(GetRegisterE(), GetRegisterD()); }
	void CPU::Execute5B() { LD_R_R(GetRegisterE(), GetRegisterE()); }
	void CPU::Execute5C() { LD_R_R(GetRegisterE(), GetRegisterH()); }
	void CPU::Execute5D() { LD_R_R(GetRegisterE(), GetRegisterL()); }
	void CPU::Execute5E() { LD_R_ADDR_RR(GetRegisterE(), GetRegisterHL()); }
	void CPU::Execute5F() { LD_R_R(GetRegisterE(), GetRegisterA()); }

	void CPU::Execute60() { LD_R_R(GetRegisterH(), GetRegisterB()); }
	void CPU::Execute61() { LD_R_R(GetRegisterH(), GetRegisterC()); }
	void CPU::Execute62() { LD_R_R(GetRegisterH(), GetRegisterD()); }
	void CPU::Execute63() { LD_R_R(GetRegisterH(), GetRegisterE()); }
	void CPU::Execute64() { LD_R_R(GetRegisterH(), GetRegisterH()); }
	void CPU::Execute65() { LD_R_R(GetRegisterH(), GetRegisterL()); }
	void CPU::Execute66() { LD_R_ADDR_RR(GetRegisterH(), GetRegisterHL()); }
	void CPU::Execute67() { LD_R_R(GetRegisterH(), GetRegisterA()); }
	void CPU::Execute68() { LD_R_R(GetRegisterL(), GetRegisterB()); }
	void CPU::Execute69() { LD_R_R(GetRegisterL(), GetRegisterC()); }
	void CPU::Execute6A() { LD_R_R(GetRegisterL(), GetRegisterD()); }
	void CPU::Execute6B() { LD_R_R(GetRegisterL(), GetRegisterE()); }
	void CPU::Execute6C() { LD_R_R(GetRegisterL(), GetRegisterH()); }
	void CPU::Execute6D() { LD_R_R(GetRegisterL(), GetRegisterL()); }
	void CPU::Execute6E() { LD_R_ADDR_RR(GetRegisterL(), GetRegisterHL()); }
	void CPU::Execute6F() { LD_R_R(GetRegisterL(), GetRegisterA()); }

	void CPU::Execute70() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterB()); }
	void CPU::Execute71() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterC()); }
	void CPU::Execute72() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterD()); }
	void CPU::Execute73() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterE()); }
	void CPU::Execute74() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterH()); }
	void CPU::Execute75() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterL()); }
	void CPU::Execute76() { HALT(); }
	void CPU::Execute77() { LD_ADDR_RR_R(GetRegisterHL(), GetRegisterA()); }
	void CPU::Execute78() { LD_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::Execute79() { LD_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::Execute7A() { LD_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::Execute7B() { LD_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::Execute7C() { LD_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::Execute7D() { LD_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::Execute7E() { LD_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute7F() { LD_R_R(GetRegisterA(), GetRegisterA()); }

	void CPU::Execute80() { ADD_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::Execute81() { ADD_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::Execute82() { ADD_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::Execute83() { ADD_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::Execute84() { ADD_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::Execute85() { ADD_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::Execute86() { ADD_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute87() { ADD_R_R(GetRegisterA(), GetRegisterA()); }
	void CPU::Execute88() { ADC_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::Execute89() { ADC_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::Execute8A() { ADC_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::Execute8B() { ADC_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::Execute8C() { ADC_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::Execute8D() { ADC_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::Execute8E() { ADC_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute8F() { ADC_R_R(GetRegisterA(), GetRegisterA()); }

	void CPU::Execute90() { SUB_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::Execute91() { SUB_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::Execute92() { SUB_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::Execute93() { SUB_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::Execute94() { SUB_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::Execute95() { SUB_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::Execute96() { SUB_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute97() { SUB_R_R(GetRegisterA(), GetRegisterA()); }
	void CPU::Execute98() { SBC_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::Execute99() { SBC_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::Execute9A() { SBC_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::Execute9B() { SBC_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::Execute9C() { SBC_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::Execute9D() { SBC_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::Execute9E() { SBC_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::Execute9F() { SBC_R_R(GetRegisterA(), GetRegisterA()); }

	void CPU::ExecuteA0() { AND_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::ExecuteA1() { AND_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::ExecuteA2() { AND_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::ExecuteA3() { AND_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::ExecuteA4() { AND_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::ExecuteA5() { AND_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::ExecuteA6() { AND_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::ExecuteA7() { AND_R_R(GetRegisterA(), GetRegisterA()); }
	void CPU::ExecuteA8() { XOR_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::ExecuteA9() { XOR_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::ExecuteAA() { XOR_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::ExecuteAB() { XOR_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::ExecuteAC() { XOR_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::ExecuteAD() { XOR_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::ExecuteAE() { XOR_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::ExecuteAF() { XOR_R_R(GetRegisterA(), GetRegisterA()); }

	void CPU::ExecuteB0() { OR_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::ExecuteB1() { OR_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::ExecuteB2() { OR_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::ExecuteB3() { OR_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::ExecuteB4() { OR_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::ExecuteB5() { OR_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::ExecuteB6() { OR_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::ExecuteB7() { OR_R_R(GetRegisterA(), GetRegisterA()); }
	void CPU::ExecuteB8() { CP_R_R(GetRegisterA(), GetRegisterB()); }
	void CPU::ExecuteB9() { CP_R_R(GetRegisterA(), GetRegisterC()); }
	void CPU::ExecuteBA() { CP_R_R(GetRegisterA(), GetRegisterD()); }
	void CPU::ExecuteBB() { CP_R_R(GetRegisterA(), GetRegisterE()); }
	void CPU::ExecuteBC() { CP_R_R(GetRegisterA(), GetRegisterH()); }
	void CPU::ExecuteBD() { CP_R_R(GetRegisterA(), GetRegisterL()); }
	void CPU::ExecuteBE() { CP_R_ADDR_RR(GetRegisterA(), GetRegisterHL()); }
	void CPU::ExecuteBF() { CP_R_R(GetRegisterA(), GetRegisterA()); }

	void CPU::ExecuteC0() { RET(!GetZeroFlag()); }
	void CPU::ExecuteC1() { POP_RR(GetRegisterBC()); }
	void CPU::ExecuteC2() { JP_U16(!GetZeroFlag()); }
	void CPU::ExecuteC3() { JP_U16(); }
	void CPU::ExecuteC4() { CALL_U16(!GetZeroFlag()); }
	void CPU::ExecuteC5() { PUSH_RR(GetRegisterBC()); }
	void CPU::ExecuteC6() { ADD_R_U8(GetRegisterA()); }
	void CPU::ExecuteC7() { RST(0x00); }
	void CPU::ExecuteC8() { RET(GetZeroFlag()); }
	void CPU::ExecuteC9() { RET(); }
	void CPU::ExecuteCA() { JP_U16(GetZeroFlag()); }
	void CPU::ExecuteCC() { CALL_U16(GetZeroFlag()); }
	void CPU::ExecuteCD() { CALL_U16(); }
	void CPU::ExecuteCE() { ADC_R_U8(GetRegisterA()); }
	void CPU::ExecuteCF() { RST(0x08); }

	void CPU::ExecuteD0() { RET(!GetCarryFlag()); }
	void CPU::ExecuteD1() { POP_RR(GetRegisterDE()); }
	void CPU::ExecuteD2() { JP_U16(!GetCarryFlag()); }
	void CPU::ExecuteD4() { CALL_U16(!GetCarryFlag()); }
	void CPU::ExecuteD5() { PUSH_RR(GetRegisterDE()); }
	void CPU::ExecuteD6() { SUB_R_U8(GetRegisterA()); }
	void CPU::ExecuteD7() { RST(0x10); }
	void CPU::ExecuteD8() { RET(GetCarryFlag()); }
	void CPU::ExecuteD9() { RETI(); }
	void CPU::ExecuteDA() { JP_U16(GetCarryFlag()); }
	void CPU::ExecuteDC() { CALL_U16(GetCarryFlag()); }
	void CPU::ExecuteDE() { SBC_R_U8(GetRegisterA()); }
	void CPU::ExecuteDF() { RST(0x18); }

	void CPU::ExecuteE0() { LD_ADDR_FF00_U8_R(GetRegisterA()); }
	void CPU::ExecuteE1() { POP_RR(GetRegisterHL()); }
	void CPU::ExecuteE2() { LD_ADDR_FF00_R_R(GetRegisterC(), GetRegisterA()); }
	void CPU::ExecuteE5() { PUSH_RR(GetRegisterHL()); }
	void CPU::ExecuteE6() { AND_R_U8(GetRegisterA()); }
	void CPU::ExecuteE7() { RST(0x20); }
	void CPU::ExecuteE8() { ADD_RR_I8(stackPointer); }
	void CPU::ExecuteE9() { JP_RR(GetRegisterHL()); }
	void CPU::ExecuteEA() { LD_ADDR_U16_R(GetRegisterA()); }
	void CPU::ExecuteEE() { XOR_R_U8(GetRegisterA()); }
	void CPU::ExecuteEF() { RST(0x28); }

	void CPU::ExecuteF0() { LD_R_ADDR_FF00_U8(GetRegisterA()); }
	void CPU::ExecuteF1() { POP_AF(); }
	void CPU::ExecuteF2() { LD_R_ADDR_FF00_R(GetRegisterA(), GetRegisterC()); }
	void CPU::ExecuteF3() { DI(); }
	void CPU::ExecuteF5() { PUSH_RR(GetRegisterAF()); }
	void CPU::ExecuteF6() { OR_R_U8(GetRegisterA()); }
	void CPU::ExecuteF7() { RST(0x30); }
	void CPU::ExecuteF8() { LD_RR_RR_I8(GetRegisterHL(), stackPointer); }
	void CPU::ExecuteF9() { LD_RR_RR(stackPointer, GetRegisterHL()); }
	void CPU::ExecuteFA() { LD_R_ADDR_U16(GetRegisterA()); }
	void CPU::ExecuteFB() { EI(); }
	void CPU::ExecuteFE() { CP_R_U8(GetRegisterA()); }
	void CPU::ExecuteFF() { RST(0x38); }

	void CPU::ExecuteCB00() { RLC_R(GetRegisterB()); }
	void CPU::ExecuteCB01() { RLC_R(GetRegisterC()); }
	void CPU::ExecuteCB02() { RLC_R(GetRegisterD()); }
	void CPU::ExecuteCB03() { RLC_R(GetRegisterE()); }
	void CPU::ExecuteCB04() { RLC_R(GetRegisterH()); }
	void CPU::ExecuteCB05() { RLC_R(GetRegisterL()); }
	void CPU::ExecuteCB06() { RLC_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB07() { RLC_R(GetRegisterA()); }
	void CPU::ExecuteCB08() { RRC_R(GetRegisterB()); }
	void CPU::ExecuteCB09() { RRC_R(GetRegisterC()); }
	void CPU::ExecuteCB0A() { RRC_R(GetRegisterD()); }
	void CPU::ExecuteCB0B() { RRC_R(GetRegisterE()); }
	void CPU::ExecuteCB0C() { RRC_R(GetRegisterH()); }
	void CPU::ExecuteCB0D() { RRC_R(GetRegisterL()); }
	void CPU::ExecuteCB0E() { RRC_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB0F() { RRC_R(GetRegisterA()); }

	void CPU::ExecuteCB10() { RL_R(GetRegisterB()); }
	void CPU::ExecuteCB11() { RL_R(GetRegisterC()); }
	void CPU::ExecuteCB12() { RL_R(GetRegisterD()); }
	void CPU::ExecuteCB13() { RL_R(GetRegisterE()); }
	void CPU::ExecuteCB14() { RL_R(GetRegisterH()); }
	void CPU::ExecuteCB15() { RL_R(GetRegisterL()); }
	void CPU::ExecuteCB16() { RL_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB17() { RL_R(GetRegisterA()); }
	void CPU::ExecuteCB18() { RR_R(GetRegisterB()); }
	void CPU::ExecuteCB19() { RR_R(GetRegisterC()); }
	void CPU::ExecuteCB1A() { RR_R(GetRegisterD()); }
	void CPU::ExecuteCB1B() { RR_R(GetRegisterE()); }
	void CPU::ExecuteCB1C() { RR_R(GetRegisterH()); }
	void CPU::ExecuteCB1D() { RR_R(GetRegisterL()); }
	void CPU::ExecuteCB1E() { RR_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB1F() { RR_R(GetRegisterA()); }

	void CPU::ExecuteCB20() { SLA_R(GetRegisterB()); }
	void CPU::ExecuteCB21() { SLA_R(GetRegisterC()); }
	void CPU::ExecuteCB22() { SLA_R(GetRegisterD()); }
	void CPU::ExecuteCB23() { SLA_R(GetRegisterE()); }
	void CPU::ExecuteCB24() { SLA_R(GetRegisterH()); }
	void CPU::ExecuteCB25() { SLA_R(GetRegisterL()); }
	void CPU::ExecuteCB26() { SLA_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB27() { SLA_R(GetRegisterA()); }
	void CPU::ExecuteCB28() { SRA_R(GetRegisterB()); }
	void CPU::ExecuteCB29() { SRA_R(GetRegisterC()); }
	void CPU::ExecuteCB2A() { SRA_R(GetRegisterD()); }
	void CPU::ExecuteCB2B() { SRA_R(GetRegisterE()); }
	void CPU::ExecuteCB2C() { SRA_R(GetRegisterH()); }
	void CPU::ExecuteCB2D() { SRA_R(GetRegisterL()); }
	void CPU::ExecuteCB2E() { SRA_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB2F() { SRA_R(GetRegisterA()); }

	void CPU::ExecuteCB30() { SWAP_R(GetRegisterB()); }
	void CPU::ExecuteCB31() { SWAP_R(GetRegisterC()); }
	void CPU::ExecuteCB32() { SWAP_R(GetRegisterD()); }
	void CPU::ExecuteCB33() { SWAP_R(GetRegisterE()); }
	void CPU::ExecuteCB34() { SWAP_R(GetRegisterH()); }
	void CPU::ExecuteCB35() { SWAP_R(GetRegisterL()); }
	void CPU::ExecuteCB36() { SWAP_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB37() { SWAP_R(GetRegisterA()); }
	void CPU::ExecuteCB38() { SRL_R(GetRegisterB()); }
	void CPU::ExecuteCB39() { SRL_R(GetRegisterC()); }
	void CPU::ExecuteCB3A() { SRL_R(GetRegisterD()); }
	void CPU::ExecuteCB3B() { SRL_R(GetRegisterE()); }
	void CPU::ExecuteCB3C() { SRL_R(GetRegisterH()); }
	void CPU::ExecuteCB3D() { SRL_R(GetRegisterL()); }
	void CPU::ExecuteCB3E() { SRL_ADDR_RR(GetRegisterHL()); }
	void CPU::ExecuteCB3F() { SRL_R(GetRegisterA()); }

	void CPU::ExecuteCB40() { BIT_N_R(0, GetRegisterB()); }
	void CPU::ExecuteCB41() { BIT_N_R(0, GetRegisterC()); }
	void CPU::ExecuteCB42() { BIT_N_R(0, GetRegisterD()); }
	void CPU::ExecuteCB43() { BIT_N_R(0, GetRegisterE()); }
	void CPU::ExecuteCB44() { BIT_N_R(0, GetRegisterH()); }
	void CPU::ExecuteCB45() { BIT_N_R(0, GetRegisterL()); }
	void CPU::ExecuteCB46() { BIT_N_ADDR_RR(0, GetRegisterHL()); }
	void CPU::ExecuteCB47() { BIT_N_R(0, GetRegisterA()); }
	void CPU::ExecuteCB48() { BIT_N_R(1, GetRegisterB()); }
	void CPU::ExecuteCB49() { BIT_N_R(1, GetRegisterC()); }
	void CPU::ExecuteCB4A() { BIT_N_R(1, GetRegisterD()); }
	void CPU::ExecuteCB4B() { BIT_N_R(1, GetRegisterE()); }
	void CPU::ExecuteCB4C() { BIT_N_R(1, GetRegisterH()); }
	void CPU::ExecuteCB4D() { BIT_N_R(1, GetRegisterL()); }
	void CPU::ExecuteCB4E() { BIT_N_ADDR_RR(1, GetRegisterHL()); }
	void CPU::ExecuteCB4F() { BIT_N_R(1, GetRegisterA()); }

	void CPU::ExecuteCB50() { BIT_N_R(2, GetRegisterB()); }
	void CPU::ExecuteCB51() { BIT_N_R(2, GetRegisterC()); }
	void CPU::ExecuteCB52() { BIT_N_R(2, GetRegisterD()); }
	void CPU::ExecuteCB53() { BIT_N_R(2, GetRegisterE()); }
	void CPU::ExecuteCB54() { BIT_N_R(2, GetRegisterH()); }
	void CPU::ExecuteCB55() { BIT_N_R(2, GetRegisterL()); }
	void CPU::ExecuteCB56() { BIT_N_ADDR_RR(2, GetRegisterHL()); }
	void CPU::ExecuteCB57() { BIT_N_R(2, GetRegisterA()); }
	void CPU::ExecuteCB58() { BIT_N_R(3, GetRegisterB()); }
	void CPU::ExecuteCB59() { BIT_N_R(3, GetRegisterC()); }
	void CPU::ExecuteCB5A() { BIT_N_R(3, GetRegisterD()); }
	void CPU::ExecuteCB5B() { BIT_N_R(3, GetRegisterE()); }
	void CPU::ExecuteCB5C() { BIT_N_R(3, GetRegisterH()); }
	void CPU::ExecuteCB5D() { BIT_N_R(3, GetRegisterL()); }
	void CPU::ExecuteCB5E() { BIT_N_ADDR_RR(3, GetRegisterHL()); }
	void CPU::ExecuteCB5F() { BIT_N_R(3, GetRegisterA()); }

	void CPU::ExecuteCB60() { BIT_N_R(4, GetRegisterB()); }
	void CPU::ExecuteCB61() { BIT_N_R(4, GetRegisterC()); }
	void CPU::ExecuteCB62() { BIT_N_R(4, GetRegisterD()); }
	void CPU::ExecuteCB63() { BIT_N_R(4, GetRegisterE()); }
	void CPU::ExecuteCB64() { BIT_N_R(4, GetRegisterH()); }
	void CPU::ExecuteCB65() { BIT_N_R(4, GetRegisterL()); }
	void CPU::ExecuteCB66() { BIT_N_ADDR_RR(4, GetRegisterHL()); }
	void CPU::ExecuteCB67() { BIT_N_R(4, GetRegisterA()); }
	void CPU::ExecuteCB68() { BIT_N_R(5, GetRegisterB()); }
	void CPU::ExecuteCB69() { BIT_N_R(5, GetRegisterC()); }
	void CPU::ExecuteCB6A() { BIT_N_R(5, GetRegisterD()); }
	void CPU::ExecuteCB6B() { BIT_N_R(5, GetRegisterE()); }
	void CPU::ExecuteCB6C() { BIT_N_R(5, GetRegisterH()); }
	void CPU::ExecuteCB6D() { BIT_N_R(5, GetRegisterL()); }
	void CPU::ExecuteCB6E() { BIT_N_ADDR_RR(5, GetRegisterHL()); }
	void CPU::ExecuteCB6F() { BIT_N_R(5, GetRegisterA()); }

	void CPU::ExecuteCB70() { BIT_N_R(6, GetRegisterB()); }
	void CPU::ExecuteCB71() { BIT_N_R(6, GetRegisterC()); }
	void CPU::ExecuteCB72() { BIT_N_R(6, GetRegisterD()); }
	void CPU::ExecuteCB73() { BIT_N_R(6, GetRegisterE()); }
	void CPU::ExecuteCB74() { BIT_N_R(6, GetRegisterH()); }
	void CPU::ExecuteCB75() { BIT_N_R(6, GetRegisterL()); }
	void CPU::ExecuteCB76() { BIT_N_ADDR_RR(6, GetRegisterHL()); }
	void CPU::ExecuteCB77() { BIT_N_R(6, GetRegisterA()); }
	void CPU::ExecuteCB78() { BIT_N_R(7, GetRegisterB()); }
	void CPU::ExecuteCB79() { BIT_N_R(7, GetRegisterC()); }
	void CPU::ExecuteCB7A() { BIT_N_R(7, GetRegisterD()); }
	void CPU::ExecuteCB7B() { BIT_N_R(7, GetRegisterE()); }
	void CPU::ExecuteCB7C() { BIT_N_R(7, GetRegisterH()); }
	void CPU::ExecuteCB7D() { BIT_N_R(7, GetRegisterL()); }
	void CPU::ExecuteCB7E() { BIT_N_ADDR_RR(7, GetRegisterHL()); }
	void CPU::ExecuteCB7F() { BIT_N_R(7, GetRegisterA()); }

	void CPU::ExecuteCB80() { RES_N_R(0, GetRegisterB()); }
	void CPU::ExecuteCB81() { RES_N_R(0, GetRegisterC()); }
	void CPU::ExecuteCB82() { RES_N_R(0, GetRegisterD()); }
	void CPU::ExecuteCB83() { RES_N_R(0, GetRegisterE()); }
	void CPU::ExecuteCB84() { RES_N_R(0, GetRegisterH()); }
	void CPU::ExecuteCB85() { RES_N_R(0, GetRegisterL()); }
	void CPU::ExecuteCB86() { RES_N_ADDR_RR(0, GetRegisterHL()); }
	void CPU::ExecuteCB87() { RES_N_R(0, GetRegisterA()); }
	void CPU::ExecuteCB88() { RES_N_R(1, GetRegisterB()); }
	void CPU::ExecuteCB89() { RES_N_R(1, GetRegisterC()); }
	void CPU::ExecuteCB8A() { RES_N_R(1, GetRegisterD()); }
	void CPU::ExecuteCB8B() { RES_N_R(1, GetRegisterE()); }
	void CPU::ExecuteCB8C() { RES_N_R(1, GetRegisterH()); }
	void CPU::ExecuteCB8D() { RES_N_R(1, GetRegisterL()); }
	void CPU::ExecuteCB8E() { RES_N_ADDR_RR(1, GetRegisterHL()); }
	void CPU::ExecuteCB8F() { RES_N_R(1, GetRegisterA()); }

	void CPU::ExecuteCB90() { RES_N_R(2, GetRegisterB()); }
	void CPU::ExecuteCB91() { RES_N_R(2, GetRegisterC()); }
	void CPU::ExecuteCB92() { RES_N_R(2, GetRegisterD()); }
	void CPU::ExecuteCB93() { RES_N_R(2, GetRegisterE()); }
	void CPU::ExecuteCB94() { RES_N_R(2, GetRegisterH()); }
	void CPU::ExecuteCB95() { RES_N_R(2, GetRegisterL()); }
	void CPU::ExecuteCB96() { RES_N_ADDR_RR(2, GetRegisterHL()); }
	void CPU::ExecuteCB97() { RES_N_R(2, GetRegisterA()); }
	void CPU::ExecuteCB98() { RES_N_R(3, GetRegisterB()); }
	void CPU::ExecuteCB99() { RES_N_R(3, GetRegisterC()); }
	void CPU::ExecuteCB9A() { RES_N_R(3, GetRegisterD()); }
	void CPU::ExecuteCB9B() { RES_N_R(3, GetRegisterE()); }
	void CPU::ExecuteCB9C() { RES_N_R(3, GetRegisterH()); }
	void CPU::ExecuteCB9D() { RES_N_R(3, GetRegisterL()); }
	void CPU::ExecuteCB9E() { RES_N_ADDR_RR(3, GetRegisterHL()); }
	void CPU::ExecuteCB9F() { RES_N_R(3, GetRegisterA()); }

	void CPU::ExecuteCBA0() { RES_N_R(4, GetRegisterB()); }
	void CPU::ExecuteCBA1() { RES_N_R(4, GetRegisterC()); }
	void CPU::ExecuteCBA2() { RES_N_R(4, GetRegisterD()); }
	void CPU::ExecuteCBA3() { RES_N_R(4, GetRegisterE()); }
	void CPU::ExecuteCBA4() { RES_N_R(4, GetRegisterH()); }
	void CPU::ExecuteCBA5() { RES_N_R(4, GetRegisterL()); }
	void CPU::ExecuteCBA6() { RES_N_ADDR_RR(4, GetRegisterHL()); }
	void CPU::ExecuteCBA7() { RES_N_R(4, GetRegisterA()); }
	void CPU::ExecuteCBA8() { RES_N_R(5, GetRegisterB()); }
	void CPU::ExecuteCBA9() { RES_N_R(5, GetRegisterC()); }
	void CPU::ExecuteCBAA() { RES_N_R(5, GetRegisterD()); }
	void CPU::ExecuteCBAB() { RES_N_R(5, GetRegisterE()); }
	void CPU::ExecuteCBAC() { RES_N_R(5, GetRegisterH()); }
	void CPU::ExecuteCBAD() { RES_N_R(5, GetRegisterL()); }
	void CPU::ExecuteCBAE() { RES_N_ADDR_RR(5, GetRegisterHL()); }
	void CPU::ExecuteCBAF() { RES_N_R(5, GetRegisterA()); }

	void CPU::ExecuteCBB0() { RES_N_R(6, GetRegisterB()); }
	void CPU::ExecuteCBB1() { RES_N_R(6, GetRegisterC()); }
	void CPU::ExecuteCBB2() { RES_N_R(6, GetRegisterD()); }
	void CPU::ExecuteCBB3() { RES_N_R(6, GetRegisterE()); }
	void CPU::ExecuteCBB4() { RES_N_R(6, GetRegisterH()); }
	void CPU::ExecuteCBB5() { RES_N_R(6, GetRegisterL()); }
	void CPU::ExecuteCBB6() { RES_N_ADDR_RR(6, GetRegisterHL()); }
	void CPU::ExecuteCBB7() { RES_N_R(6, GetRegisterA()); }
	void CPU::ExecuteCBB8() { RES_N_R(7, GetRegisterB()); }
	void CPU::ExecuteCBB9() { RES_N_R(7, GetRegisterC()); }
	void CPU::ExecuteCBBA() { RES_N_R(7, GetRegisterD()); }
	void CPU::ExecuteCBBB() { RES_N_R(7, GetRegisterE()); }
	void CPU::ExecuteCBBC() { RES_N_R(7, GetRegisterH()); }
	void CPU::ExecuteCBBD() { RES_N_R(7, GetRegisterL()); }
	void CPU::ExecuteCBBE() { RES_N_ADDR_RR(7, GetRegisterHL()); }
	void CPU::ExecuteCBBF() { RES_N_R(7, GetRegisterA()); }

	void CPU::ExecuteCBC0() { SET_N_R(0, GetRegisterB()); }
	void CPU::ExecuteCBC1() { SET_N_R(0, GetRegisterC()); }
	void CPU::ExecuteCBC2() { SET_N_R(0, GetRegisterD()); }
	void CPU::ExecuteCBC3() { SET_N_R(0, GetRegisterE()); }
	void CPU::ExecuteCBC4() { SET_N_R(0, GetRegisterH()); }
	void CPU::ExecuteCBC5() { SET_N_R(0, GetRegisterL()); }
	void CPU::ExecuteCBC6() { SET_N_ADDR_RR(0, GetRegisterHL()); }
	void CPU::ExecuteCBC7() { SET_N_R(0, GetRegisterA()); }
	void CPU::ExecuteCBC8() { SET_N_R(1, GetRegisterB()); }
	void CPU::ExecuteCBC9() { SET_N_R(1, GetRegisterC()); }
	void CPU::ExecuteCBCA() { SET_N_R(1, GetRegisterD()); }
	void CPU::ExecuteCBCB() { SET_N_R(1, GetRegisterE()); }
	void CPU::ExecuteCBCC() { SET_N_R(1, GetRegisterH()); }
	void CPU::ExecuteCBCD() { SET_N_R(1, GetRegisterL()); }
	void CPU::ExecuteCBCE() { SET_N_ADDR_RR(1, GetRegisterHL()); }
	void CPU::ExecuteCBCF() { SET_N_R(1, GetRegisterA()); }

	void CPU::ExecuteCBD0() { SET_N_R(2, GetRegisterB()); }
	void CPU::ExecuteCBD1() { SET_N_R(2, GetRegisterC()); }
	void CPU::ExecuteCBD2() { SET_N_R(2, GetRegisterD()); }
	void CPU::ExecuteCBD3() { SET_N_R(2, GetRegisterE()); }
	void CPU::ExecuteCBD4() { SET_N_R(2, GetRegisterH()); }
	void CPU::ExecuteCBD5() { SET_N_R(2, GetRegisterL()); }
	void CPU::ExecuteCBD6() { SET_N_ADDR_RR(2, GetRegisterHL()); }
	void CPU::ExecuteCBD7() { SET_N_R(2, GetRegisterA()); }
	void CPU::ExecuteCBD8() { SET_N_R(3, GetRegisterB()); }
	void CPU::ExecuteCBD9() { SET_N_R(3, GetRegisterC()); }
	void CPU::ExecuteCBDA() { SET_N_R(3, GetRegisterD()); }
	void CPU::ExecuteCBDB() { SET_N_R(3, GetRegisterE()); }
	void CPU::ExecuteCBDC() { SET_N_R(3, GetRegisterH()); }
	void CPU::ExecuteCBDD() { SET_N_R(3, GetRegisterL()); }
	void CPU::ExecuteCBDE() { SET_N_ADDR_RR(3, GetRegisterHL()); }
	void CPU::ExecuteCBDF() { SET_N_R(3, GetRegisterA()); }

	void CPU::ExecuteCBE0() { SET_N_R(4, GetRegisterB()); }
	void CPU::ExecuteCBE1() { SET_N_R(4, GetRegisterC()); }
	void CPU::ExecuteCBE2() { SET_N_R(4, GetRegisterD()); }
	void CPU::ExecuteCBE3() { SET_N_R(4, GetRegisterE()); }
	void CPU::ExecuteCBE4() { SET_N_R(4, GetRegisterH()); }
	void CPU::ExecuteCBE5() { SET_N_R(4, GetRegisterL()); }
	void CPU::ExecuteCBE6() { SET_N_ADDR_RR(4, GetRegisterHL()); }
	void CPU::ExecuteCBE7() { SET_N_R(4, GetRegisterA()); }
	void CPU::ExecuteCBE8() { SET_N_R(5, GetRegisterB()); }
	void CPU::ExecuteCBE9() { SET_N_R(5, GetRegisterC()); }
	void CPU::ExecuteCBEA() { SET_N_R(5, GetRegisterD()); }
	void CPU::ExecuteCBEB() { SET_N_R(5, GetRegisterE()); }
	void CPU::ExecuteCBEC() { SET_N_R(5, GetRegisterH()); }
	void CPU::ExecuteCBED() { SET_N_R(5, GetRegisterL()); }
	void CPU::ExecuteCBEE() { SET_N_ADDR_RR(5, GetRegisterHL()); }
	void CPU::ExecuteCBEF() { SET_N_R(5, GetRegisterA()); }

	void CPU::ExecuteCBF0() { SET_N_R(6, GetRegisterB()); }
	void CPU::ExecuteCBF1() { SET_N_R(6, GetRegisterC()); }
	void CPU::ExecuteCBF2() { SET_N_R(6, GetRegisterD()); }
	void CPU::ExecuteCBF3() { SET_N_R(6, GetRegisterE()); }
	void CPU::ExecuteCBF4() { SET_N_R(6, GetRegisterH()); }
	void CPU::ExecuteCBF5() { SET_N_R(6, GetRegisterL()); }
	void CPU::ExecuteCBF6() { SET_N_ADDR_RR(6, GetRegisterHL()); }
	void CPU::ExecuteCBF7() { SET_N_R(6, GetRegisterA()); }
	void CPU::ExecuteCBF8() { SET_N_R(7, GetRegisterB()); }
	void CPU::ExecuteCBF9() { SET_N_R(7, GetRegisterC()); }
	void CPU::ExecuteCBFA() { SET_N_R(7, GetRegisterD()); }
	void CPU::ExecuteCBFB() { SET_N_R(7, GetRegisterE()); }
	void CPU::ExecuteCBFC() { SET_N_R(7, GetRegisterH()); }
	void CPU::ExecuteCBFD() { SET_N_R(7, GetRegisterL()); }
	void CPU::ExecuteCBFE() { SET_N_ADDR_RR(7, GetRegisterHL()); }
	void CPU::ExecuteCBFF() { SET_N_R(7, GetRegisterA()); }
}