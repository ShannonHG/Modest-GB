#include <cassert>
#include <string>
#include <sstream>
#include "Globals.hpp"
#include "CPU/CPU.hpp"
#include "Logger.hpp"
#include "InvalidOpcodeException.hpp"
#include <iomanip>

namespace SHG
{
	CPU::CPU(MemoryManagementUnit& memoryManagementUnit) : memoryManagementUnit(memoryManagementUnit)
	{
		this->memoryManagementUnit = memoryManagementUnit;
		registers = std::map<CPURegisterID, CPURegister>();

		registers[CPURegisterID::AF] = CPURegister();
		registers[CPURegisterID::BC] = CPURegister();
		registers[CPURegisterID::HL] = CPURegister();
		registers[CPURegisterID::DE] = CPURegister();
		registers[CPURegisterID::PC] = CPURegister();
		registers[CPURegisterID::SP] = CPURegister();
	}

	uint32_t CPU::Cycle()
	{
		uint8_t opcode = Fetch8Bit();
		CPUInstruction instruction = Decode(opcode);

		std::stringstream messageStream;

		if (instruction.instructionType == CPUInstructionType::Invalid)
		{
			messageStream << "[CPU] Invalid opcode encountered: " << ConvertToHexString(opcode, 2);
			throw InvalidOpcodeException(messageStream.str());
		}

		// Log information about the instruction 
		messageStream << "[CPU] Executing instruction - Opcode: " << ConvertToHexString(opcode, 2) << " | " << "Data: ";
		for (auto data : instruction.data) messageStream << data << " ";

		Logger::Write(messageStream.str());

		return Execute(instruction);
	}

	uint8_t CPU::Fetch8Bit()
	{
		uint16_t address = registers[CPURegisterID::PC].GetData();
		registers[CPURegisterID::PC].Increment();

		if (!memoryManagementUnit.IsAddressAvailable(address))
		{
			std::stringstream messageStream;
			messageStream << "[CPU] Failed to fetch data from " << ConvertToHexString(address, 4) << ". Memory address is inaccessible.";

			throw std::out_of_range(messageStream.str());
		}

		uint8_t result = memoryManagementUnit.GetByte(address);

		return result;
	}

	uint16_t CPU::Fetch16Bit()
	{
		uint8_t lowerByte = Fetch8Bit();
		uint8_t upperByte = Fetch8Bit();

		return (upperByte << 8) | lowerByte;
	}

	CPUInstruction CPU::Decode(uint8_t opcode)
	{
		// TODO: Set duration of instruction

		CPUInstruction instruction;

		if (opcode >= 0x40 && opcode < 0x80)
		{
			instruction = DecodeLoadAndStoreInstruction(opcode);
		}
		else if (opcode >= 0x80 && opcode < 0xC0)
		{
			instruction = DecodeArithmeticInstruction(opcode);
		}
		else
		{
			uint8_t data8;
			uint16_t data16;

			switch (opcode)
			{
			case 0x00:
				break;
			case 0x01:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::BC];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				data16 = Fetch16Bit();
				instruction.data.push_back(data16);
				break;
			case 0x02:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::BC];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte());
				break;
			case 0x03:
				break;
			case 0x04:
				break;
			case 0x05:
				break;
			case 0x06:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::BC];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x07:
				break;
			case 0x08:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				data16 = Fetch16Bit();
				instruction.targetAddress = data16;
				instruction.data.push_back(registers[CPURegisterID::SP].GetData());
				break;
			case 0x09:
				break;
			case 0x0A:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				instruction.data.push_back(registers[CPURegisterID::BC].GetData());
				break;
			case 0x0B:
				break;
			case 0x0C:
				break;
			case 0x0D:
				break;
			case 0x0E:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::BC];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Lower;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x0F:
				break;
			case 0x10:
				break;
			case 0x11:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::DE];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				data16 = Fetch16Bit();
				instruction.data.push_back(data16);
				break;
			case 0x12:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::DE];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte());
				break;
			case 0x13:
				break;
			case 0x14:
				break;
			case 0x15:
				break;
			case 0x16:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::DE];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x17:
				break;
			case 0x18:
				break;
			case 0x19:
				break;
			case 0x1A:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				instruction.data.push_back(registers[CPURegisterID::DE].GetData());
				break;
			case 0x1B:
				break;
			case 0x1C:
				break;
			case 0x1D:
				break;
			case 0x1E:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::DE];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Lower;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x1F:
				break;
			case 0x20:
				break;
			case 0x21:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::HL];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				data16 = Fetch16Bit();
				instruction.data.push_back(data16);
				break;
			case 0x22:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::HL];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte() + 1);
				break;
			case 0x23:
				break;
			case 0x24:
				break;
			case 0x25:
				break;
			case 0x26:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::HL];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x27:
				break;
			case 0x28:
				break;
			case 0x29:
				break;
			case 0x2A:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				instruction.data.push_back(registers[CPURegisterID::HL].GetData());
				registers[CPURegisterID::HL].Increment();
				break;
			case 0x2B:
				break;
			case 0x2C:
				break;
			case 0x2D:
				break;
			case 0x2E:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::HL];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Lower;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x2F:
				break;
			case 0x30:
				break;
			case 0x31:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::SP];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				data16 = Fetch16Bit();
				instruction.data.push_back(data16);
				break;
			case 0x32:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::HL];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte() - 1);
				break;
			case 0x33:
				break;
			case 0x34:
				break;
			case 0x35:
				break;
			case 0x36:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::HL];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x37:
				break;
			case 0x38:
				break;
			case 0x39:
				break;
			case 0x3A:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				instruction.data.push_back(registers[CPURegisterID::HL].GetData());
				registers[CPURegisterID::HL].Decrement();
				break;
			case 0x3B:
				break;
			case 0x3C:
				break;
			case 0x3D:
				break;
			case 0x3E:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data8 = Fetch8Bit();
				instruction.data.push_back(data8);
				break;
			case 0x3F:
				break;
			case 0xC0:
				break;
			case 0xC1:
				break;
			case 0xC2:
				break;
			case 0xC3:
				break;
			case 0xC4:
				break;
			case 0xC5:
				break;
			case 0xC6:
				break;
			case 0xC7:
				break;
			case 0xC8:
				break;
			case 0xC9:
				break;
			case 0xCA:
				break;
			case 0xCB:
				break;
			case 0xCC:
				break;
			case 0xCD:
				break;
			case 0xCE:
				break;
			case 0xCF:
				break;
			case 0xD0:
				break;
			case 0xD1:
				break;
			case 0xD2:
				break;
			case 0xD3:
				break;
			case 0xD4:
				break;
			case 0xD5:
				break;
			case 0xD6:
				break;
			case 0xD7:
				break;
			case 0xD8:
				break;
			case 0xD9:
				break;
			case 0xDA:
				break;
			case 0xDB:
				break;
			case 0xDC:
				break;
			case 0xDD:
				break;
			case 0xDE:
				break;
			case 0xDF:
				break;
			case 0xE0:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				data8 = Fetch8Bit();
				instruction.targetAddress = 0xFF00 | data8;
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte());
				break;
			case 0xE1:
				break;
			case 0xE2:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.targetAddress = 0xFF00 | registers[CPURegisterID::BC].GetLowerByte();
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte());
				break;
			case 0xE3:
				break;
			case 0xE4:
				break;
			case 0xE5:
				break;
			case 0xE6:
				break;
			case 0xE7:
				break;
			case 0xE8:
				break;
			case 0xE9:
				break;
			case 0xEA:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
				data16 = Fetch16Bit();
				instruction.targetAddress = data16;
				instruction.data.push_back(registers[CPURegisterID::AF].GetUpperByte());
				break;
			case 0xEB:
				break;
			case 0xEC:
				break;
			case 0xED:
				break;
			case 0xEE:
				break;
			case 0xEF:
				break;
			case 0xF0:
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data8 = Fetch8Bit();
				data8 = memoryManagementUnit.GetByte(0xFF00 | data8);
				instruction.data.push_back(data8);
				break;
			case 0xF1:
				break;
			case 0xF2:
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data8 = memoryManagementUnit.GetByte(0xFF00 | registers[CPURegisterID::BC].GetLowerByte());
				instruction.data.push_back(data8);
				break;
			case 0xF3:
				break;
			case 0xF4:
				break;
			case 0xF5:
				break;
			case 0xF6:
				break;
			case 0xF7:
				break;
			case 0xF8:
				break;
			case 0xF9:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister = &registers[CPURegisterID::SP];
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Full;
				instruction.data.push_back(registers[CPURegisterID::HL].GetData());
				break;
			case 0xFA:
				instruction.targetRegister = &registers[CPURegisterID::AF];
				instruction.instructionType = CPUInstructionType::Load;
				instruction.operandSize = CPUInstructionOperandSize::EightBit;
				instruction.registerAddressType = CPURegisterAddressType::Upper;
				data16 = Fetch16Bit();
				data8 = memoryManagementUnit.GetByte(data16);
				instruction.data.push_back(data8);
				break;
			case 0xFB:
				break;
			case 0xFC:
				break;
			case 0xFD:
				break;
			case 0xFE:
				break;
			case 0xFF:
				break;
			default:
				instruction.instructionType = CPUInstructionType::Invalid;
				break;
			}
		}

		return instruction;
	}

	uint32_t CPU::Execute(const CPUInstruction& instruction)
	{
		CPURegister* targetRegister = instruction.targetRegister;
		CPURegisterAddressType addressType = instruction.registerAddressType;

		uint16_t result = 0;

		switch (instruction.instructionType)
		{
		case CPUInstructionType::Add:
			result = instruction.data[0] + instruction.data[1];
			break;
		case CPUInstructionType::AddWithCarry:
			break;
		case CPUInstructionType::AND:
			result = instruction.data[0] & instruction.data[1];
			break;
		case CPUInstructionType::DecimalAdjust:
			break;
		case CPUInstructionType::Decrement:
			break;
		case CPUInstructionType::Increment:
			break;
		case CPUInstructionType::Load:
			result = instruction.data[0];
			break;
		case CPUInstructionType::OR:
			result = instruction.data[0] | instruction.data[1];
			break;
		case CPUInstructionType::Subtract:
			result = instruction.data[0] - instruction.data[1];
			break;
		case CPUInstructionType::SubtractWithCarry:
			break;
		case CPUInstructionType::XOR:
			result = instruction.data[0] ^ instruction.data[1];
			break;
		}

		switch (instruction.operandSize)
		{
		case CPUInstructionOperandSize::EightBit:
			if (instruction.targetRegister == NULL) memoryManagementUnit.SetByte(instruction.targetAddress, instruction.data[0]);
			else StoreValueInRegister(instruction.targetRegister, instruction.registerAddressType, (uint8_t)result);
			break;
		case CPUInstructionOperandSize::SixteenBit:
			if (instruction.targetRegister == NULL)
			{
				uint8_t upperByte = instruction.data[0] >> 8;
				uint8_t lowerByte = instruction.data[0] & 0x00FF;
				memoryManagementUnit.SetByte(instruction.targetAddress, lowerByte);
				memoryManagementUnit.SetByte(instruction.targetAddress + 1, upperByte);
			}
			else StoreValueInRegister(instruction.targetRegister, instruction.registerAddressType, result);
			break;
		}

		return instruction.duration;
	}

	void CPU::StoreValueInRegister(CPURegister* targetRegister, CPURegisterAddressType addressType, uint16_t value)
	{
		switch (addressType)
		{
		case CPURegisterAddressType::Lower:
			targetRegister->SetLowerByte((uint8_t)value);
			break;
		case CPURegisterAddressType::Upper:
			targetRegister->SetUpperByte((uint8_t)value);
			break;
		case CPURegisterAddressType::Full:
			targetRegister->SetData(value);
			break;
		}
	}

	CPURegister& CPU::GetRegister(CPURegisterID registerID)
	{
		// The CPU should always have a corresponding register for every CPURegisterID.
		assert(registers.count(registerID) != 0);
		return registers[registerID];
	}

	CPUInstruction CPU::DecodeLoadAndStoreInstruction(uint8_t opcode)
	{
		CPUInstruction instruction;

		uint8_t upperNibble = opcode >> 4;
		uint8_t lowerNibble = opcode & 0x0F;

		instruction.instructionType = CPUInstructionType::Load;

		// Since there are a total of 7 different registers that may be provided as the source
		// register (B, C, D, E, H, L, A), only 3 bits are needed to identify the register. 
		// This keeps the decoded value between 0 - 7 (an 8 will be interpreted as 0, and a 9 as 1).
		uint8_t lower3Bits = lowerNibble & 0b111;

		instruction.operandSize = CPUInstructionOperandSize::EightBit;

		uint8_t data = 0;

		switch (lower3Bits)
		{
		case 6:
			instruction.operandSize = CPUInstructionOperandSize::SixteenBit;
			data = registers[CPURegisterID::HL].GetData();
			break;
		case 7:
			data = registers[CPURegisterID::AF].GetUpperByte();
			break;
		default:
			// Registers have the following mapping:
			// B: 0
			// C: 1
			// D: 2
			// E: 3
			// H: 4
			// L: 5
			// Since the registers are stored in the "registers" map as 16-bit registers, 
			// we need to index them using the index of their upper 8-bit register, which 
			// happens to have an even index. In addition, since there are only 8 possible registers, 
			// but 16 possible values, each register ID will appear twice (B can be represented by both 0 and 8).
			// With this in mind, the source register's ID can generally be determined using the least
			// significant 3 bits, subtracted by the remainder of the lower nibble divided by 2 (which provides 
			// us with the index of its upper 8-bit register). This makes more sense when looking at an opcode table. 
			// Additionally, this does not work for determining the 8-bit A register, or the 16-bit HL register,
			// so those are handled manually.
			auto sourceRegisterID = (CPURegisterID)(lower3Bits - (lowerNibble % 2));

			// If the value of the least signicant 4 bits is even, then get the data from the uppermost 8-bit register, 
			// otherwise use the data in the lower 8-bit register.
			data = lowerNibble % 2 == 0 ? registers[sourceRegisterID].GetUpperByte() : registers[sourceRegisterID].GetLowerByte();
			break;
		}

		instruction.data.push_back(data);

		CPURegisterID targetRegisterID;

		if (upperNibble == 7)
		{
			targetRegisterID = lowerNibble <= 7 ? CPURegisterID::HL : CPURegisterID::AF;
			instruction.registerAddressType = targetRegisterID == CPURegisterID::HL ? CPURegisterAddressType::Full : CPURegisterAddressType::Upper;
		}
		else
		{
			// Due to the format of the opcodes in this range, subtracting the upper 4-bits from 8, 
			// happens to give us the correct target register ID.
			targetRegisterID = (CPURegisterID)(upperNibble - (8 - upperNibble));;
			instruction.registerAddressType = lowerNibble <= 7 ? CPURegisterAddressType::Upper : CPURegisterAddressType::Lower;
		}

		instruction.targetRegister = &registers[targetRegisterID];
		return instruction;
	}

	CPUInstruction CPU::DecodeArithmeticInstruction(uint8_t opcode)
	{
		CPUInstruction instruction;
		return instruction;
	}
}