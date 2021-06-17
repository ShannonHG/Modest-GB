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
	// Some opcode's can have their instruction type easily decoded by checking 
	// if their upper nibble is less than or greater than a certain threshold.
	// If their upper nibble is less than or equal to the threshold, one instruction type is chosen,
	// otherwise another instruction type is chosen. By normalizing the upper nibble between 0 and 1,
	// this map can be used to retrieve the appropriate instruction type.
	std::map<uint8_t, std::array<CPUInstructionType, 2>> instructionTypeMap =
	{
		{ 0x08, {CPUInstructionType::Add, CPUInstructionType::AddWithCarry}},
		{ 0x09, {CPUInstructionType::Subtract, CPUInstructionType::SubtractWithCarry}},
		{ 0x0A, {CPUInstructionType::AND, CPUInstructionType::XOR}},
		{ 0x0B, {CPUInstructionType::OR, CPUInstructionType::Compare}}
	};

	// Similar to the above map, but used specifically for certain CB prefixed opcodes.
	std::map<uint8_t, std::array<CPUInstructionType, 2>> cbPrefixedInstructionTypeMap =
	{
		{ 0x00, {CPUInstructionType::RotateLeft, CPUInstructionType::RotateRight}},
		{ 0x01, {CPUInstructionType::RotateLeftThroughCarry, CPUInstructionType::RotateRightThroughCarry}},
		{ 0x02, {CPUInstructionType::ArithmeticLeftShift, CPUInstructionType::ArithmeticRightShift}},
		{ 0x03, {CPUInstructionType::Swap, CPUInstructionType::LogicalRightShift}}
	};

	CPU::CPU(MemoryManagementUnit& memoryManagementUnit) : memoryManagementUnit(memoryManagementUnit)
	{
		this->memoryManagementUnit = memoryManagementUnit;
		registers = std::map<CPURegisterID, Register16>();

		registers[CPURegisterID::AF] = Register16();
		regAF = &registers[CPURegisterID::AF];

		registers[CPURegisterID::BC] = Register16();
		regBC = &registers[CPURegisterID::BC];

		registers[CPURegisterID::HL] = Register16();
		regHL = &registers[CPURegisterID::HL];

		registers[CPURegisterID::DE] = Register16();
		regDE = &registers[CPURegisterID::DE];

		registers[CPURegisterID::PC] = Register16();
		programCounter = &registers[CPURegisterID::PC];

		registers[CPURegisterID::SP] = Register16();
		stackPointer = &registers[CPURegisterID::SP];
	}

	uint32_t CPU::Cycle()
	{
		Logger::Write("[CPU] Fetching byte...");
		uint8_t opcode = Fetch8Bit();

		Logger::Write("[CPU] Decoding instruction...");
		CPUInstruction instruction = Decode(opcode);

		std::stringstream messageStream;

		if (instruction.instructionType == CPUInstructionType::Invalid)
		{
			messageStream << "[CPU] Invalid opcode encountered: " << ConvertToHexString(instruction.opcode, 2);
			throw InvalidOpcodeException(messageStream.str());
		}

		// Log information about the instruction 
		messageStream << "[CPU] Executing instruction - ";
		messageStream << instruction.ToString();

		Logger::Write(messageStream.str());

		previouslyExecutedInstruction = instruction;
		return Execute(instruction);
	}

	uint8_t CPU::Fetch8Bit()
	{
		uint16_t address = programCounter->GetData();
		programCounter->Increment();

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
		instruction.opcode = opcode;

		if (opcode >= 0x40 && opcode < 0x80)
		{
			DecodeLoadAndStoreInstruction(instruction);
		}
		else if (opcode >= 0x80 && opcode < 0xC0)
		{
			DecodeArithmeticInstruction(instruction);
		}
		else
		{
			switch (opcode)
			{
			case 0x00:
				// NOP
				instruction.instructionType = CPUInstructionType::NOP;
				instruction.storageType = CPUInstructionStorageType::None;
				break;
			case 0x01:
				Create16BitLoadInstruction(instruction, regBC, Fetch16Bit());
				break;
			case 0x02:
				Create8BitLoadInstruction(instruction, regBC->GetData(), regAF->GetHighByte());
				break;
			case 0x03:
				Create16BitIncrementInstruction(instruction, regBC);
				break;
			case 0x04:
				Create8BitIncrementInstruction(instruction, &regBC->GetHighRegister());
				break;
			case 0x05:
				Create8BitDecrementInstruction(instruction, &regBC->GetHighRegister());
				break;
			case 0x06:
				Create8BitLoadInstruction(instruction, &regBC->GetHighRegister(), Fetch8Bit());
				break;
			case 0x07:
				// RLCA
				instruction.instructionType = CPUInstructionType::RotateLeft;
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
				instruction.targetRegister8 = GetRegisterA();
				instruction.data.push_back(instruction.targetRegister8->GetData());
				break;
			case 0x08:
				Create16BitLoadInstruction(instruction, Fetch16Bit(), stackPointer->GetData());
				break;
			case 0x09:
				Create16BitAddInstruction(instruction, regHL, regBC->GetData());
				break;
			case 0x0A:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(regBC->GetData()));
				break;
			case 0x0B:
				Create16BitDecrementInstruction(instruction, regBC);
				break;
			case 0x0C:
				Create8BitIncrementInstruction(instruction, &regBC->GetLowRegister());
				break;
			case 0x0D:
				Create8BitDecrementInstruction(instruction, &regBC->GetLowRegister());
				break;
			case 0x0E:
				Create8BitLoadInstruction(instruction, &regBC->GetLowRegister(), Fetch8Bit());
				break;
			case 0x0F:
				instruction.instructionType = CPUInstructionType::RotateRight;
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
				instruction.targetRegister8 = GetRegisterA();
				instruction.data.push_back(instruction.targetRegister8->GetData());
				break;
			case 0x10:
				// TODO: Confirm whether some additional logic should be here
				// STOP
				instruction.instructionType = CPUInstructionType::STOP;
				instruction.storageType = CPUInstructionStorageType::None;
				break;
			case 0x11:
				Create16BitLoadInstruction(instruction, regDE, Fetch16Bit());
				break;
			case 0x12:
				Create8BitLoadInstruction(instruction, regDE->GetData(), regAF->GetHighByte());
				break;
			case 0x13:
				Create16BitIncrementInstruction(instruction, regDE);
				break;
			case 0x14:
				Create8BitIncrementInstruction(instruction, &regDE->GetHighRegister());
				break;
			case 0x15:
				Create8BitDecrementInstruction(instruction, &regDE->GetHighRegister());
				break;
			case 0x16:
				Create8BitLoadInstruction(instruction, &regDE->GetHighRegister(), Fetch8Bit());
				break;
			case 0x17:
				instruction.instructionType = CPUInstructionType::RotateLeftThroughCarry;
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
				instruction.targetRegister8 = GetRegisterA();
				instruction.data.push_back(instruction.targetRegister8->GetData());
				break;
			case 0x18:
				CreateRelativeJumpInstruction(instruction, Fetch8Bit(), {});
				break;
			case 0x19:
				Create16BitAddInstruction(instruction, regHL, regDE->GetData());
				break;
			case 0x1A:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(regDE->GetData()));
				break;
			case 0x1B:
				Create16BitDecrementInstruction(instruction, regDE);
				break;
			case 0x1C:
				Create8BitIncrementInstruction(instruction, &regDE->GetLowRegister());
				break;
			case 0x1D:
				Create8BitDecrementInstruction(instruction, &regDE->GetLowRegister());
				break;
			case 0x1E:
				Create8BitLoadInstruction(instruction, &regDE->GetLowRegister(), Fetch8Bit());
				break;
			case 0x1F:
				instruction.instructionType = CPUInstructionType::RotateRightThroughCarry;
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
				instruction.targetRegister8 = GetRegisterA();
				instruction.data.push_back(instruction.targetRegister8->GetData());
				break;
			case 0x20:
				CreateRelativeJumpInstruction(instruction, Fetch8Bit(), { CPUFlag::Subtraction, CPUFlag::Zero });
				break;
			case 0x21:
				Create16BitLoadInstruction(instruction, regHL, Fetch16Bit());
				break;
			case 0x22:
				Create8BitLoadInstruction(instruction, regHL->GetData(), regAF->GetHighByte());
				regHL->Increment();
				break;
			case 0x23:
				Create16BitIncrementInstruction(instruction, regHL);
				break;
			case 0x24:
				Create8BitIncrementInstruction(instruction, &regHL->GetHighRegister());
				break;
			case 0x25:
				Create8BitDecrementInstruction(instruction, &regHL->GetHighRegister());
				break;
			case 0x26:
				Create8BitLoadInstruction(instruction, &regHL->GetHighRegister(), Fetch8Bit());
				break;
			case 0x27:
				instruction.instructionType = CPUInstructionType::DecimalAdjust;
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
				instruction.targetRegister8 = &regAF->GetHighRegister();
				instruction.data.push_back(regAF->GetHighByte());
				break;
			case 0x28:
				CreateRelativeJumpInstruction(instruction, Fetch8Bit(), { CPUFlag::Zero });
				break;
			case 0x29:
				Create16BitAddInstruction(instruction, regHL, regHL->GetData());
				break;
			case 0x2A:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(regHL->GetData()));
				regHL->Increment();
				break;
			case 0x2B:
				Create16BitDecrementInstruction(instruction, regHL);
				break;
			case 0x2C:
				Create8BitIncrementInstruction(instruction, &regHL->GetLowRegister());
				break;
			case 0x2D:
				Create8BitDecrementInstruction(instruction, &regHL->GetLowRegister());
				break;
			case 0x2E:
				Create8BitLoadInstruction(instruction, &regHL->GetLowRegister(), Fetch8Bit());
				break;
			case 0x2F:
				instruction.instructionType = CPUInstructionType::OnesComplement;
				instruction.targetRegister8 = &regAF->GetHighRegister();
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
				instruction.data.push_back(regAF->GetHighByte());
				break;
			case 0x30:
				CreateRelativeJumpInstruction(instruction, Fetch8Bit(), { CPUFlag::Subtraction, CPUFlag::Carry });
				break;
			case 0x31:
				Create16BitLoadInstruction(instruction, stackPointer, Fetch16Bit());
				break;
			case 0x32:
				Create8BitLoadInstruction(instruction, regHL->GetData(), regAF->GetHighByte());
				regHL->Decrement();
				break;
			case 0x33:
				Create16BitIncrementInstruction(instruction, stackPointer);
				break;
			case 0x34:
				Create8BitIncrementInstruction(instruction, regHL->GetData());
				break;
			case 0x35:
				Create8BitDecrementInstruction(instruction, regHL->GetData());
				break;
			case 0x36:
				Create8BitLoadInstruction(instruction, regHL->GetData(), Fetch8Bit());
				break;
			case 0x37:
				instruction.instructionType = CPUInstructionType::SetCarryFlag;
				instruction.storageType = CPUInstructionStorageType::None;
				break;
			case 0x38:
				CreateRelativeJumpInstruction(instruction, Fetch8Bit(), { CPUFlag::Carry });
				break;
			case 0x39:
				Create16BitAddInstruction(instruction, regHL, stackPointer->GetData());
				break;
			case 0x3A:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(regHL->GetData()));
				regHL->Decrement();
				break;
			case 0x3B:
				Create16BitDecrementInstruction(instruction, stackPointer);
				break;
			case 0x3C:
				Create8BitIncrementInstruction(instruction, &regAF->GetHighRegister());
				break;
			case 0x3D:
				Create8BitDecrementInstruction(instruction, &regAF->GetHighRegister());
				break;
			case 0x3E:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0x3F:
				instruction.instructionType = CPUInstructionType::FlipCarryFlag;
				instruction.storageType = CPUInstructionStorageType::None;
				break;
			case 0xC0:
				CreateReturnInstruction(instruction, { CPUFlag::Subtraction, CPUFlag::Zero });
				break;
			case 0xC1:
				CreatePopInstruction(instruction, regBC->GetData());
				break;
			case 0xC2:
				CreateStandardJumpInstruction(instruction, Fetch16Bit(), { CPUFlag::Subtraction, CPUFlag::Zero });
				break;
			case 0xC3:
				CreateStandardJumpInstruction(instruction, Fetch16Bit(), {});
				break;
			case 0xC4:
				CreateCallInstruction(instruction, Fetch16Bit(), { CPUFlag::Subtraction, CPUFlag::Zero });
				break;
			case 0xC5:
				CreatePushInstruction(instruction, regBC->GetData());
				break;
			case 0xC6:
				Create8BitAddInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xC7:
				CreateRestartInstruction(instruction, 0x00);
				break;
			case 0xC8:
				CreateReturnInstruction(instruction, { CPUFlag::Zero });
				break;
			case 0xC9:
				CreateReturnInstruction(instruction, {});
				break;
			case 0xCA:
				CreateStandardJumpInstruction(instruction, Fetch16Bit(), { CPUFlag::Zero });
				break;
			case 0xCB:
				instruction.opcode = (opcode << 8) | Fetch8Bit();
				// Another byte is fetched to determine which CB prefixed opcode was encountered.
				DecodeCBPrefixedInstruction(instruction);
				break;
			case 0xCC:
				CreateCallInstruction(instruction, Fetch16Bit(), { CPUFlag::Zero });
				break;
			case 0xCD:
				CreateCallInstruction(instruction, Fetch16Bit(), {});
				break;
			case 0xCE:
				Create8BitAddWithCarryInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xCF:
				CreateRestartInstruction(instruction, 0x08);
				break;
			case 0xD0:
				CreateReturnInstruction(instruction, { CPUFlag::Subtraction, CPUFlag::Carry });
				break;
			case 0xD1:
				CreatePopInstruction(instruction, regDE->GetData());
				break;
			case 0xD2:
				CreateStandardJumpInstruction(instruction, Fetch16Bit(), { CPUFlag::Subtraction, CPUFlag::Carry });
				break;
			case 0xD3:
				// Nothing
				break;
			case 0xD4:
				CreateCallInstruction(instruction, Fetch16Bit(), { CPUFlag::Subtraction, CPUFlag::Carry });
				break;
			case 0xD5:
				CreatePushInstruction(instruction, regDE->GetData());
				break;
			case 0xD6:
				Create8BitSubtractInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xD7:
				CreateRestartInstruction(instruction, 0x10);
				break;
			case 0xD8:
				CreateReturnInstruction(instruction, { CPUFlag::Carry });
				break;
			case 0xD9:
				instruction.instructionType = CPUInstructionType::ReturnAndEnableInterrupts;
				instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
				instruction.targetRegister16 = programCounter;
				break;
			case 0xDA:
				CreateStandardJumpInstruction(instruction, Fetch16Bit(), { CPUFlag::Carry });
				break;
			case 0xDB:
				// Nothing
				break;
			case 0xDC:
				CreateCallInstruction(instruction, Fetch16Bit(), { CPUFlag::Carry });
				break;
			case 0xDD:
				// Nothing
				break;
			case 0xDE:
				Create8BitSubtractWithCarryInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xDF:
				CreateRestartInstruction(instruction, 0x18);
				break;
			case 0xE0:
				Create8BitLoadInstruction(instruction, 0xFF00 | Fetch8Bit(), regAF->GetHighByte());
				break;
			case 0xE1:
				CreatePopInstruction(instruction, regHL->GetData());
				break;
			case 0xE2:
				Create8BitLoadInstruction(instruction, 0xFF00 | regBC->GetLowByte(), regAF->GetHighByte());
				break;
			case 0xE3:
				// Nothing
				break;
			case 0xE4:
				// Nothing
				break;
			case 0xE5:
				CreatePushInstruction(instruction, regHL->GetData());
				break;
			case 0xE6:
				Create8BitANDInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xE7:
				CreateRestartInstruction(instruction, 0x20);
				break;
			case 0xE8:
				instruction.instructionType = CPUInstructionType::Add;
				instruction.targetRegister16 = stackPointer;
				instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
				instruction.data.push_back(stackPointer->GetData());
				instruction.data.push_back((int8_t)Fetch8Bit());
				break;
			case 0xE9:
				CreateStandardJumpInstruction(instruction, regHL->GetData(), {});
				break;
			case 0xEA:
				Create8BitLoadInstruction(instruction, Fetch16Bit(), regAF->GetHighByte());
				break;
			case 0xEB:
				// Nothing
				break;
			case 0xEC:
				// Nothing
				break;
			case 0xED:
				// Nothing
				break;
			case 0xEE:
				Create8BitXORInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xEF:
				CreateRestartInstruction(instruction, 0x28);
				break;
			case 0xF0:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(0xFF00 | Fetch8Bit()));
				break;
			case 0xF1:
				CreatePopInstruction(instruction, regAF->GetData());
				break;
			case 0xF2:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(0xFF00 | regBC->GetLowByte()));
				break;
			case 0xF3:
				CreateInterruptDisableInstruction(instruction);
				break;
			case 0xF4:
				// Nothing
				break;
			case 0xF5:
				CreatePushInstruction(instruction, regAF->GetData());
				break;
			case 0xF6:
				Create8BitORInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xF7:
				CreateRestartInstruction(instruction, 0x30);
				break;
			case 0xF8:
				instruction.instructionType = CPUInstructionType::Load;
				instruction.targetRegister16 = regHL;
				instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
				instruction.data.push_back(stackPointer->GetData() + ((int8_t)Fetch8Bit()));
				break;
			case 0xF9:
				Create16BitLoadInstruction(instruction, stackPointer, regHL->GetData());
				break;
			case 0xFA:
				Create8BitLoadInstruction(instruction, &regAF->GetHighRegister(), memoryManagementUnit.GetByte(Fetch16Bit()));
				break;
			case 0xFB:
				CreateInterruptEnableInstruction(instruction);
				break;
			case 0xFC:
				// Nothing
				break;
			case 0xFD:
				// Nothing
				break;
			case 0xFE:
				Create8BitCompareInstruction(instruction, &regAF->GetHighRegister(), Fetch8Bit());
				break;
			case 0xFF:
				CreateRestartInstruction(instruction, 0x38);
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
		// TODO: This can probably remain an uint
		// int is used instead of uint here so that
		// the result can be checked for a negative value. 
		// Otherwise when the result of an operation is less than zero, 
		// an uint would return 255 instead.
		int result = 0;
		bool shouldEnableHalfCarryFlag = false;
		bool shouldEnableCarryFlag = false;
		bool shouldEnableSubtractionFlag = false;
		bool shouldEnableZeroFlag = false;

		switch (instruction.instructionType)
		{
		case CPUInstructionType::Add:
			result = instruction.data[0] + instruction.data[1];
			shouldEnableZeroFlag = result == 0;
			shouldEnableCarryFlag = ShouldEnableCarryFlag(instruction, result);
			shouldEnableHalfCarryFlag = ShouldEnableHalfCarryFlag(instruction.data[0], instruction.data[1]);
			break;
		case CPUInstructionType::AddWithCarry:
		{
			uint8_t carryFlag = GetCarryFlag();
			result = instruction.data[0] + instruction.data[1] + carryFlag;
			shouldEnableZeroFlag = result == 0;
			shouldEnableCarryFlag = ShouldEnableCarryFlag(instruction, result);
			shouldEnableHalfCarryFlag = ShouldEnableHalfCarryFlag(instruction.data[0], (instruction.data[1] + carryFlag));
			break;
		}
		case CPUInstructionType::AND:
			result = instruction.data[0] & instruction.data[1];
			shouldEnableZeroFlag = result == 0;
			break;
		case CPUInstructionType::DecimalAdjust:
		{
			result = instruction.data[0];

			// Adjust the result so that it becomes a Binary-coded Decimal
			uint8_t lowerNibble = instruction.data[0] & 0x0F;
			if (lowerNibble >= 0x0A || (GetHalfCarryFlag() == 1))
			{
				result += 0x06;
				shouldEnableHalfCarryFlag = ShouldEnableHalfCarryFlag(lowerNibble, 0x06);
			}

			// Adjust the result so that it becomes a Binary-coded Decimal
			uint8_t upperNibble = result >> 4;
			if (upperNibble >= 0xA0 || (GetCarryFlag() == 1))
			{
				result += 0x60;
				shouldEnableCarryFlag = result < 0 || result > std::numeric_limits<uint8_t>::max();
			}
			break;
		}
		case CPUInstructionType::Decrement:
			result = instruction.data[0] - 1;
			shouldEnableZeroFlag = result == 0;
			break;
		case CPUInstructionType::Increment:
			result = instruction.data[0] + 1;
			break;
		case CPUInstructionType::Load:
			result = instruction.data[0];
			break;
		case CPUInstructionType::OR:
			result = instruction.data[0] | instruction.data[1];
			shouldEnableZeroFlag = result == 0;
			break;
		case CPUInstructionType::Subtract:
			result = instruction.data[0] - instruction.data[1];
			shouldEnableZeroFlag = result == 0;
			shouldEnableCarryFlag = ShouldEnableCarryFlag(instruction, result);
			shouldEnableHalfCarryFlag = ShouldEnableHalfCarryFlag(instruction.data[0], -instruction.data[1]);
			break;
		case CPUInstructionType::SubtractWithCarry:
		{
			uint8_t carryFlag = GetCarryFlag();
			result = instruction.data[0] - instruction.data[1] - carryFlag;
			shouldEnableZeroFlag = result == 0;
			shouldEnableCarryFlag = ShouldEnableCarryFlag(instruction, result);
			shouldEnableHalfCarryFlag = ShouldEnableHalfCarryFlag(instruction.data[0], -(instruction.data[1] + carryFlag));
			break;
		}
		case CPUInstructionType::XOR:
			result = instruction.data[0] ^ instruction.data[1];
			shouldEnableZeroFlag = result == 0;
			break;
		case CPUInstructionType::Compare:
			result = instruction.data[0] - instruction.data[1];
			shouldEnableZeroFlag = result == 0;
			break;
		case CPUInstructionType::FlipCarryFlag:
			shouldEnableCarryFlag = GetCarryFlag() ^ 1;
			break;
		case CPUInstructionType::SetCarryFlag:
			shouldEnableCarryFlag = true;
			break;
		case CPUInstructionType::OnesComplement:
			result = instruction.data[0] ^ 0x00FF;
			break;
		case CPUInstructionType::RotateLeft:
			result = (instruction.data[0] << 1) | (instruction.data[0] >> 7);
			break;
		case CPUInstructionType::RotateRight:
			result = (instruction.data[0] >> 1) | (instruction.data[0] << 7);
			break;
		case CPUInstructionType::RotateLeftThroughCarry:
			shouldEnableCarryFlag = instruction.data[0] & 0b10000000;
			result = (instruction.data[0] << 1) | (GetCarryFlag());
			break;
		case CPUInstructionType::RotateRightThroughCarry:
			shouldEnableCarryFlag = instruction.data[0] & 1;
			result = (instruction.data[0] >> 1) | (GetCarryFlag() << 7);
			break;
		case CPUInstructionType::ArithmeticLeftShift:
			result = instruction.data[0] << 1;
			break;
		case CPUInstructionType::ArithmeticRightShift:
			// Shift right while retaining the sign bit (most significant bit)
			result = (instruction.data[0] >> 1) & (instruction.data[0] & 0b10000000);
			break;
		case CPUInstructionType::LogicalRightShift:
			result = instruction.data[0] >> 1;
			break;
		case CPUInstructionType::Swap:
			// Swap the low and high nibbles
			result = ((instruction.data[0] & 0x0F) << 4) | ((instruction.data[0] & 0xF0) >> 4);
			break;
		case CPUInstructionType::BitTest:
			shouldEnableZeroFlag = ((instruction.data[0]) | (1 << instruction.data[1])) >> instruction.data[1];
			break;
		case CPUInstructionType::BitReset:
			result = (instruction.data[0]) & ~(1 << instruction.data[1]);
			break;
		case CPUInstructionType::BitSet:
			result = (instruction.data[0]) | (1 << instruction.data[1]);
			break;
		case CPUInstructionType::Jump:
			result = instruction.data[0];
			break;
		case CPUInstructionType::RelativeJump:
			result = instruction.targetRegister16->GetData() + instruction.data[0];
			break;
		case CPUInstructionType::Return:
			result = memoryManagementUnit.GetByte(stackPointer->GetData()) | (memoryManagementUnit.GetByte(stackPointer->GetData() + 1) << 8);
			stackPointer->Increase(2);
			break;
		case CPUInstructionType::Restart:
		case CPUInstructionType::Call:
			stackPointer->Decrease(2);
			Set16BitDataInMemory(stackPointer->GetData(), programCounter->GetData());
			result = instruction.data[0];
			break;
		case CPUInstructionType::EnableInterrupts:
			interruptMasterEnableFlag = 1;
			break;
		case CPUInstructionType::DisableInterrupts:
			interruptMasterEnableFlag = 0;
			break;
		case CPUInstructionType::ReturnAndEnableInterrupts:
			interruptMasterEnableFlag = 1;
			result = memoryManagementUnit.GetByte(stackPointer->GetData()) | (memoryManagementUnit.GetByte(stackPointer->GetData() + 1) << 8);
			stackPointer->Increase(2);
			break;
		}

		SetSubtractionFlag(shouldEnableSubtractionFlag);
		SetZeroFlag(shouldEnableZeroFlag);
		SetCarryFlag(shouldEnableCarryFlag);
		SetHalfCarryFlag(shouldEnableHalfCarryFlag);

		switch (instruction.storageType)
		{
		case CPUInstructionStorageType::EightBitMemoryData:
			assert(instruction.targetAddress != NULL);
			memoryManagementUnit.SetByte(instruction.targetAddress, result);
			break;
		case CPUInstructionStorageType::SixteenBitMemoryData:
			assert(instruction.targetAddress != NULL);
			Set16BitDataInMemory(instruction.targetAddress, result);
			break;
		case CPUInstructionStorageType::EightBitRegisterData:
			assert(instruction.targetRegister8 != NULL);
			instruction.targetRegister8->SetData((uint8_t)result);
			break;
		case CPUInstructionStorageType::SixteenBitRegisterData:
			assert(instruction.targetRegister16 != NULL);
			instruction.targetRegister16->SetData(result);
			break;
		}

		return instruction.duration;
	}

	void CPU::DecodeLoadAndStoreInstruction(CPUInstruction& instruction)
	{
		instruction.instructionType = CPUInstructionType::Load;

		uint8_t upperNibble = instruction.opcode >> 4;
		uint8_t lowerNibble = instruction.opcode & 0x0F;

		instruction.data.push_back(GenerateDataFromOpcode(instruction.opcode));

		CPURegisterID targetRegisterID;

		if (upperNibble == 7)
		{
			if (lowerNibble <= 7)
			{
				instruction.targetAddress = regHL->GetData();
				instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
			}
			else
			{
				instruction.targetRegister8 = &regAF->GetHighRegister();
				instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
			}
		}
		else
		{
			instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;

			// Due to the format of the opcodes in this range, subtracting the upper 4-bits from 8, 
			// happens to give us the correct target register ID.
			targetRegisterID = (CPURegisterID)(upperNibble - (8 - upperNibble));
			Register16* sixteenBitRegister = &registers[targetRegisterID];

			instruction.targetRegister8 = lowerNibble <= 7 ? &sixteenBitRegister->GetHighRegister() : &sixteenBitRegister->GetLowRegister();

		}
	}

	void CPU::DecodeArithmeticInstruction(CPUInstruction& instruction)
	{
		instruction.targetRegister8 = &regAF->GetHighRegister();
		instruction.data.push_back(regAF->GetHighByte());

		uint8_t upperNibble = instruction.opcode >> 4;
		uint8_t lowerNibble = instruction.opcode & 0x0F;

		instruction.data.push_back(GenerateDataFromOpcode(instruction.opcode));
		instruction.instructionType = instructionTypeMap[upperNibble][lowerNibble <= 7 ? 0 : 1];

		switch (instruction.instructionType)
		{
		case CPUInstructionType::Compare:
			instruction.storageType = CPUInstructionStorageType::None;
			break;
		default:
			instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
			break;
		}
	}

	void CPU::DecodeCBPrefixedInstruction(CPUInstruction& instruction)
	{
		uint8_t opcodeWithoutPrefix = instruction.opcode & 0x00FF;
		instruction.data.push_back(GenerateDataFromOpcode(opcodeWithoutPrefix));

		uint8_t upperNibble = opcodeWithoutPrefix >> 4;
		uint8_t lowNibble = opcodeWithoutPrefix & 0x0F;
		uint8_t lower3Bits = lowNibble & 0b111;

		uint8_t normalizedLowNibble = lowNibble <= 7 ? 0 : 1;
		uint8_t bitIndex = (upperNibble - (8 - upperNibble)) + normalizedLowNibble;
		Logger::Write("Bit Index: " + std::to_string(opcodeWithoutPrefix));
		switch (lower3Bits)
		{
		case 6:
			instruction.targetAddress = regHL->GetData();
			instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
			break;
		case 7:
			instruction.targetRegister8 = &regAF->GetHighRegister();
			instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
			break;
		default:
			instruction.targetRegister8 = Get8BitRegisterFromOpcode(instruction.opcode);
			instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
			break;
		}

		switch (upperNibble)
		{
		default:
			// Handles RLC, RRC, RL, RR, SLA, SRA, SWAP, and SRL
			instruction.instructionType = cbPrefixedInstructionTypeMap[upperNibble][normalizedLowNibble];
			break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			// BIT
			instruction.instructionType = CPUInstructionType::BitTest;
			instruction.storageType = CPUInstructionStorageType::None;
			instruction.data.push_back((upperNibble - (4 - (upperNibble % 4))) + normalizedLowNibble);
			break;
		case 0x08:
		case 0x09:
		case 0x0A:
		case 0x0B:
			// RES
			instruction.instructionType = CPUInstructionType::BitReset;
			instruction.data.push_back((upperNibble - (8 - (upperNibble % 8))) + normalizedLowNibble);
			break;
		case 0x0C:
		case 0x0D:
		case 0x0E:
		case 0x0F:
			// SET
			instruction.instructionType = CPUInstructionType::BitSet;
			instruction.data.push_back((upperNibble - (12 - (upperNibble % 12))) + normalizedLowNibble);
			break;
		}
	}

	uint8_t CPU::GenerateDataFromOpcode(uint8_t opcode)
	{
		uint8_t lowerNibble = opcode & 0x0F;
		uint8_t lower3Bits = lowerNibble & 0b111;

		uint16_t data = 0;

		switch (lower3Bits)
		{
		case 6:
			data = memoryManagementUnit.GetByte(regHL->GetData());
			break;
		case 7:
			data = regAF->GetHighByte();
			break;
		default:
			data = Get8BitRegisterFromOpcode(opcode)->GetData();
			break;
		}

		return data;
	}

	Register8* CPU::Get8BitRegisterFromOpcode(uint8_t opcode)
	{
		uint8_t lowerNibble = opcode & 0x0F;

		// Since there are a total of 7 different registers that may be provided as the source
		// register (B, C, D, E, H, L, A), only 3 bits are needed to identify the register. 
		// This keeps the decoded value between 0 - 7 (an 8 will be interpreted as 0, and a 9 as 1).
		uint8_t lower3Bits = lowerNibble & 0b111;

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
		return lowerNibble % 2 == 0 ? &registers[sourceRegisterID].GetHighRegister() : &registers[sourceRegisterID].GetLowRegister();
	}

	void CPU::Create8BitIncrementInstruction(CPUInstruction& instruction, Register8* targetRegister)
	{
		instruction.instructionType = CPUInstructionType::Increment;
		instruction.targetRegister8 = targetRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(targetRegister->GetData());
	}

	void CPU::Create8BitIncrementInstruction(CPUInstruction& instruction, uint16_t targetAddress)
	{
		instruction.instructionType = CPUInstructionType::Increment;
		instruction.targetAddress = targetAddress;
		instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
		instruction.data.push_back(memoryManagementUnit.GetByte(targetAddress));
	}

	void CPU::Create16BitIncrementInstruction(CPUInstruction& instruction, Register16* targetRegister)
	{
		instruction.instructionType = CPUInstructionType::Increment;
		instruction.targetRegister16 = targetRegister;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.data.push_back(targetRegister->GetData());
	}

	void CPU::Create16BitIncrementInstruction(CPUInstruction& instruction, uint16_t targetAddress)
	{
		instruction.instructionType = CPUInstructionType::Increment;
		instruction.targetAddress = targetAddress;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.data.push_back(memoryManagementUnit.GetByte(targetAddress));
	}

	void CPU::Create8BitDecrementInstruction(CPUInstruction& instruction, Register8* targetRegister)
	{
		instruction.instructionType = CPUInstructionType::Decrement;
		instruction.targetRegister8 = targetRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(targetRegister->GetData());
	}

	void CPU::Create16BitDecrementInstruction(CPUInstruction& instruction, Register16* targetRegister)
	{
		instruction.instructionType = CPUInstructionType::Decrement;
		instruction.targetRegister16 = targetRegister;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.data.push_back(targetRegister->GetData());
	}

	void CPU::Create8BitDecrementInstruction(CPUInstruction& instruction, uint16_t targetAddress)
	{
		instruction.instructionType = CPUInstructionType::Decrement;
		instruction.targetAddress = targetAddress;
		instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
		instruction.data.push_back(memoryManagementUnit.GetByte(targetAddress));
	}

	void CPU::Create8BitLoadInstruction(CPUInstruction& instruction, Register8* targetRegister, uint8_t data)
	{
		instruction.instructionType = CPUInstructionType::Load;
		instruction.targetRegister8 = targetRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(data);
	}

	void CPU::Create8BitLoadInstruction(CPUInstruction& instruction, uint16_t targetAddress, uint8_t data)
	{
		instruction.instructionType = CPUInstructionType::Load;
		instruction.targetAddress = targetAddress;
		instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
		instruction.data.push_back(data);
	}

	void CPU::Create16BitLoadInstruction(CPUInstruction& instruction, Register16* targetRegister, uint16_t data)
	{
		instruction.instructionType = CPUInstructionType::Load;
		instruction.targetRegister16 = targetRegister;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.data.push_back(data);
	}

	void CPU::Create16BitLoadInstruction(CPUInstruction& instruction, uint16_t targetAddress, uint16_t data)
	{
		instruction.instructionType = CPUInstructionType::Load;
		instruction.targetAddress = targetAddress;
		instruction.storageType = CPUInstructionStorageType::SixteenBitMemoryData;
		instruction.data.push_back(data);
	}

	void CPU::Create8BitAddInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::Add;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create16BitAddInstruction(CPUInstruction& instruction, Register16* storageRegister, uint16_t operand)
	{
		instruction.instructionType = CPUInstructionType::Add;
		instruction.targetRegister16 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitAddWithCarryInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::AddWithCarry;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitSubtractInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::Subtract;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitSubtractWithCarryInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::SubtractWithCarry;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitXORInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::XOR;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitORInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::OR;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitCompareInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::Compare;
		instruction.storageType = CPUInstructionStorageType::None;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::Create8BitANDInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand)
	{
		instruction.instructionType = CPUInstructionType::AND;
		instruction.targetRegister8 = storageRegister;
		instruction.storageType = CPUInstructionStorageType::EightBitRegisterData;
		instruction.data.push_back(storageRegister->GetData());
		instruction.data.push_back(operand);
	}

	void CPU::CreateStandardJumpInstruction(CPUInstruction& instruction, uint16_t data, std::vector<CPUFlag> flags)
	{
		instruction.instructionType = CPUInstructionType::Jump;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.targetRegister16 = programCounter;
		instruction.flags = flags;
		instruction.data.push_back(data);
	}

	void CPU::CreateRelativeJumpInstruction(CPUInstruction& instruction, uint8_t data, std::vector<CPUFlag> flags)
	{
		instruction.instructionType = CPUInstructionType::RelativeJump;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.targetRegister16 = programCounter;
		instruction.flags = flags;
		instruction.data.push_back(data);
	}

	void CPU::CreateCallInstruction(CPUInstruction& instruction, uint16_t data, std::vector<CPUFlag> flags)
	{
		instruction.instructionType = CPUInstructionType::Call;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.targetRegister16 = programCounter;
		instruction.flags = flags;
		instruction.data.push_back(data);
	}

	void CPU::CreateRestartInstruction(CPUInstruction& instruction, uint8_t data)
	{
		instruction.instructionType = CPUInstructionType::Restart;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.targetRegister16 = programCounter;
		instruction.data.push_back(data);
	}

	void CPU::CreateReturnInstruction(CPUInstruction& instruction, std::vector<CPUFlag> flags)
	{
		instruction.instructionType = CPUInstructionType::Return;
		instruction.storageType = CPUInstructionStorageType::SixteenBitRegisterData;
		instruction.targetRegister16 = programCounter;
		instruction.flags = flags;
	}

	void CPU::CreatePopInstruction(CPUInstruction& instruction, uint16_t data)
	{
		instruction.instructionType = CPUInstructionType::Pop;
		instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
		instruction.data.push_back(data);
	}

	void CPU::CreatePushInstruction(CPUInstruction& instruction, uint16_t data)
	{
		instruction.instructionType = CPUInstructionType::Push;
		instruction.storageType = CPUInstructionStorageType::EightBitMemoryData;
		instruction.data.push_back(data);
	}

	void CPU::CreateInterruptDisableInstruction(CPUInstruction& instruction)
	{
		assert(false);
	}

	void CPU::CreateInterruptEnableInstruction(CPUInstruction& instruction)
	{
		assert(false);
	}

	bool CPU::ShouldEnableCarryFlag(const CPUInstruction& instruction, int operationResult)
	{
		switch (instruction.instructionType)
		{
		case CPUInstructionType::Add:
		case CPUInstructionType::AddWithCarry:
			switch (instruction.storageType)
			{
			case CPUInstructionStorageType::EightBitMemoryData:
			case CPUInstructionStorageType::EightBitRegisterData:
				return operationResult > std::numeric_limits<uint8_t>::max();
			case CPUInstructionStorageType::SixteenBitMemoryData:
			case CPUInstructionStorageType::SixteenBitRegisterData:
				return operationResult > std::numeric_limits<uint16_t>::max();
				break;

			}
		case CPUInstructionType::Subtract:
		case CPUInstructionType::SubtractWithCarry:
			return operationResult < 0;
		default:
			return false;
		}
	}

	bool CPU::ShouldEnableHalfCarryFlag(uint16_t operand1, uint16_t operand2)
	{
		// Isolate and add low nibbles to determine if a carry occurs.
		return (((operand1 & 0x0F) + (operand2 & 0x0F)) & 0x10) == 0x10;
	}

	Register8* CPU::GetRegisterA()
	{
		return &regAF->GetHighRegister();
	}
	Register8* CPU::GetRegisterF()
	{
		return &regAF->GetLowRegister();
	}

	Register8* CPU::GetRegisterB()
	{
		return &regBC->GetHighRegister();
	}

	Register8* CPU::GetRegisterC()
	{
		return &regBC->GetLowRegister();
	}

	Register8* CPU::GetRegisterD()
	{
		return &regDE->GetHighRegister();
	}

	Register8* CPU::GetRegisterE()
	{
		return &regDE->GetLowRegister();
	}

	Register8* CPU::GetRegisterH()
	{
		return &regHL->GetHighRegister();
	}

	Register8* CPU::GetRegisterL()
	{
		return &regHL->GetLowRegister();
	}

	Register16* CPU::GetRegisterAF()
	{
		return regAF;
	}

	Register16* CPU::GetRegisterBC()
	{
		return regBC;
	}

	Register16* CPU::GetRegisterDE()
	{
		return regDE;
	}

	Register16* CPU::GetRegisterHL()
	{
		return regHL;
	}

	Register16* CPU::GetProgramCounter()
	{
		return programCounter;
	}

	Register16* CPU::GetStackPointer()
	{
		return stackPointer;
	}

	Register16& CPU::GetRegister(CPURegisterID registerID)
	{
		// The CPU should always have a corresponding register for every CPURegisterID.
		assert(registers.count(registerID) != 0);
		return registers[registerID];
	}

	uint8_t CPU::GetZeroFlag()
	{
		return regAF->GetLowRegister().GetBit(7);
	}

	uint8_t CPU::GetSubtractionFlag()
	{
		return regAF->GetLowRegister().GetBit(6);
	}

	uint8_t CPU::GetHalfCarryFlag()
	{
		return regAF->GetLowRegister().GetBit(5);
	}

	uint8_t CPU::GetCarryFlag()
	{
		return regAF->GetLowRegister().GetBit(4);
	}

	void CPU::SetZeroFlag(bool enabled)
	{
		regAF->GetLowRegister().SetBit(7, enabled);
	}

	void CPU::SetSubtractionFlag(bool enabled)
	{
		regAF->GetLowRegister().SetBit(6, enabled);
	}

	void CPU::SetHalfCarryFlag(bool enabled)
	{
		regAF->GetLowRegister().SetBit(5, enabled);
	}

	void CPU::SetCarryFlag(bool enabled)
	{
		regAF->GetLowRegister().SetBit(4, enabled);
	}

	void CPU::Set16BitDataInMemory(uint16_t address, uint16_t data)
	{
		memoryManagementUnit.SetByte(address, data & 0x00FF);
		memoryManagementUnit.SetByte(address + 1, data >> 8);
	}

	CPUInstruction CPU::GetPreviouslyExecutedInstruction()
	{
		return previouslyExecutedInstruction;
	}

	bool CPU::GetInterruptMasterEnableFlag()
	{
		return interruptMasterEnableFlag;
	}
}