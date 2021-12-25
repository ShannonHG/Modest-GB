#include <filesystem>
#include "Memory/MemoryMap.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "InvalidOpcodeException.hpp"
#include "Utils/MemoryUtils.hpp"

namespace SHG
{
	const std::string MEMORY_MAP_LOG_HEADER = "[Memory Map]";

	uint8_t MemoryMap::Read(uint16_t address) const
	{
		if (Arithmetic::IsInRange(address, GB_ROM_BANK_00_START_ADDRESS, GB_ROM_BANK_00_END_ADDRESS))
		{
			return cartridge->Read(address);
		}
		else if (Arithmetic::IsInRange(address, GB_SWITCHABLE_ROM_BANK_START_ADDRESS, GB_SWITCHABLE_ROM_BANK_END_ADDRESS))
		{
			return cartridge->Read(address);
		}
		else if (Arithmetic::IsInRange(address, GB_VRAM_START_ADDRESS, GB_VRAM_END_ADDRESS))
		{
			return vram->Read(Arithmetic::Normalize(address, GB_VRAM_START_ADDRESS, GB_VRAM_END_ADDRESS));
		}
		else if (Arithmetic::IsInRange(address, GB_EXTERNAL_RAM_START_ADDRESS, GB_EXTERNAL_RAM_END_ADDRESS))
		{
			return cartridge->Read(address);
		}
		else if (Arithmetic::IsInRange(address, GB_WORK_RAM_START_ADDRESS, GB_WORK_RAM_END_ADDRESS))
		{
			return wram->Read(Arithmetic::Normalize(address, GB_WORK_RAM_START_ADDRESS, GB_WORK_RAM_END_ADDRESS));
		}
		else if (Arithmetic::IsInRange(address, GB_ECHO_RAM_START_ADDRESS, GB_ECHO_RAM_END_ADDRESS))
		{
			return echoRam->Read(Arithmetic::Normalize(address, GB_ECHO_RAM_START_ADDRESS, GB_ECHO_RAM_END_ADDRESS));
		}
		else if (Arithmetic::IsInRange(address, GB_OAM_START_ADDRESS, GB_OAM_END_ADDRESS))
		{
			return oam->Read(Arithmetic::Normalize(address, GB_OAM_START_ADDRESS, GB_OAM_END_ADDRESS));
		}
		else if (Arithmetic::IsInRange(address, GB_UNUSABLE_MEMORY_START_ADDRESS, GB_UNUSABLE_MEMORY_END_ADDRESS))
		{
			return restrictedMemory->Read(Arithmetic::Normalize(address, GB_UNUSABLE_MEMORY_START_ADDRESS, GB_UNUSABLE_MEMORY_END_ADDRESS));
		}
		else if (Arithmetic::IsInRange(address, GB_IO_REGISTERS_START_ADDRESS, GB_IO_REGISTERS_END_ADDRESS))
		{
			return ReadIO(address);
		}
		else if (Arithmetic::IsInRange(address, GB_HIGH_RAM_START_ADDRESS, GB_HIGH_RAM_END_ADDRESS))
		{
			return hram->Read(Arithmetic::Normalize(address, GB_HIGH_RAM_START_ADDRESS, GB_HIGH_RAM_END_ADDRESS));
		}
		else if (address == GB_INTERRUPT_ENABLE_ADDRESS)
		{
			return interruptEnableRegister->Read();
		}
		else
		{
			Logger::WriteError("Attempted to read from invalid address: " + std::to_string(address), MEMORY_MAP_LOG_HEADER);
			return 0;
		}
	}

	void MemoryMap::Write(uint16_t address, uint8_t value)
	{
		if (Arithmetic::IsInRange(address, GB_ROM_BANK_00_START_ADDRESS, GB_ROM_BANK_00_END_ADDRESS))
		{
			cartridge->Write(address, value);
		}
		else if (Arithmetic::IsInRange(address, GB_SWITCHABLE_ROM_BANK_START_ADDRESS, GB_SWITCHABLE_ROM_BANK_END_ADDRESS))
		{
			cartridge->Write(address, value);
		}
		else if (Arithmetic::IsInRange(address, GB_VRAM_START_ADDRESS, GB_VRAM_END_ADDRESS))
		{
			vram->Write(Arithmetic::Normalize(address, GB_VRAM_START_ADDRESS, GB_VRAM_END_ADDRESS), value);
		}
		else if (Arithmetic::IsInRange(address, GB_EXTERNAL_RAM_START_ADDRESS, GB_EXTERNAL_RAM_END_ADDRESS))
		{
			cartridge->Write(address, value);
		}
		else if (Arithmetic::IsInRange(address, GB_WORK_RAM_START_ADDRESS, GB_WORK_RAM_END_ADDRESS))
		{
			wram->Write(Arithmetic::Normalize(address, GB_WORK_RAM_START_ADDRESS, GB_WORK_RAM_END_ADDRESS), value);
		}
		else if (Arithmetic::IsInRange(address, GB_ECHO_RAM_START_ADDRESS, GB_ECHO_RAM_END_ADDRESS))
		{
			echoRam->Write(Arithmetic::Normalize(address, GB_ECHO_RAM_START_ADDRESS, GB_ECHO_RAM_END_ADDRESS), value);
		}
		else if (Arithmetic::IsInRange(address, GB_OAM_START_ADDRESS, GB_OAM_END_ADDRESS))
		{
			oam->Write(Arithmetic::Normalize(address, GB_OAM_START_ADDRESS, GB_OAM_END_ADDRESS), value);
		}
		else if (Arithmetic::IsInRange(address, GB_UNUSABLE_MEMORY_START_ADDRESS, GB_UNUSABLE_MEMORY_END_ADDRESS))
		{
			restrictedMemory->Write(Arithmetic::Normalize(address, GB_UNUSABLE_MEMORY_START_ADDRESS, GB_UNUSABLE_MEMORY_END_ADDRESS), value);
		}
		else if (Arithmetic::IsInRange(address, GB_IO_REGISTERS_START_ADDRESS, GB_IO_REGISTERS_END_ADDRESS))
		{
			WriteIO(address, value);
		}
		else if (Arithmetic::IsInRange(address, GB_HIGH_RAM_START_ADDRESS, GB_HIGH_RAM_END_ADDRESS))
		{
			hram->Write(Arithmetic::Normalize(address, GB_HIGH_RAM_START_ADDRESS, GB_HIGH_RAM_END_ADDRESS), value);
		}
		else if (address == GB_INTERRUPT_ENABLE_ADDRESS)
		{
			interruptEnableRegister->Write(value);
		}
		else
		{
			Logger::WriteError("Attempted to write to invalid address: " + std::to_string(address), MEMORY_MAP_LOG_HEADER);
		}
	}

	void MemoryMap::Reset()
	{
		joypad->Write(0xCF);
		Write(0xFF01, 0x00);
		Write(0xFF02, 0x7E);
		Write(0xFF04, 0xAB);
		Write(0xFF05, 0x00);
		Write(0xFF06, 0x00);
		Write(0xFF07, 0xF8);
		Write(0xFF0F, 0xE1);
		Write(0xFF10, 0x80);
		Write(0xFF11, 0xBF);
		Write(0xFF12, 0xF3);
		Write(0xFF14, 0xBF);
		Write(0xFF16, 0x3F);
		Write(0xFF17, 0x00);
		Write(0xFF18, 0xFF);
		Write(0xFF19, 0xBF);
		Write(0xFF1A, 0x7F);
		Write(0xFF1B, 0xFF);
		Write(0xFF1C, 0x9F);
		Write(0xFF1D, 0xFF);
		Write(0xFF1E, 0xBF);
		Write(0xFF20, 0xFF);
		Write(0xFF21, 0x00);
		Write(0xFF22, 0x00);
		Write(0xFF23, 0xBF);
		Write(0xFF24, 0x77);
		Write(0xFF25, 0xF3);
		Write(0xFF26, 0xF1);
		Write(0xFF40, 0x91);
		Write(0xFF41, 0x85);
		Write(0xFF42, 0x00);
		Write(0xFF43, 0x00);
		ppu->GetLY().Write(0);
		Write(0xFF45, 0x00);
		Write(0xFF46, 0xFF);
		Write(0xFF47, 0xFC);
		Write(0xFF48, 0xFF);
		Write(0xFF49, 0xFF);
		Write(0xFF4A, 0x00);
		Write(0xFF4B, 0x00);
		Write(0xFF4D, 0xFF);
		Write(0xFF4F, 0xFF);
		Write(0xFF51, 0xFF);
		Write(0xFF52, 0xFF);
		Write(0xFF53, 0xFF);
		Write(0xFF54, 0xFF);
		Write(0xFF55, 0xFF);
		Write(0xFF56, 0xFF);
		Write(0xFF68, 0xFF);
		Write(0xFF69, 0xFF);
		Write(0xFF6A, 0xFF);
		Write(0xFF6B, 0xFF);
		Write(0xFF70, 0xFF);
		Write(0xFFFF, 0x00);
	}

	bool MemoryMap::IsAddressAvailable(uint16_t address) const
	{
		// TODO: Revisit
		return true;
	}

	void MemoryMap::AttachCartridge(Cartridge* cartridge)
	{
		this->cartridge = cartridge;
	}

	void MemoryMap::AttachPPU(PPU* ppu)
	{
		this->ppu = ppu;
	}

	void MemoryMap::AttachTimer(Timer* timer)
	{
		this->timer = timer;
	}

	void MemoryMap::AttachVRAM(BasicMemory* vram)
	{
		this->vram = vram;
	}

	void MemoryMap::AttachWRAM(BasicMemory* wram)
	{
		this->wram = wram;
	}

	void MemoryMap::AttachHRAM(BasicMemory* hram)
	{
		this->hram = hram;
	}

	void MemoryMap::AttachOAM(BasicMemory* oam)
	{
		this->oam = oam;
	}

	void MemoryMap::AttachEchoRAM(BasicMemory* echoRam)
	{
		this->echoRam = echoRam;
	}

	void MemoryMap::AttachGenericIO(BasicMemory* ioRegisters)
	{
		this->ioRegisters = ioRegisters;
	}

	void MemoryMap::AttachInterruptEnableRegister(Register8* interruptEnableRegister)
	{
		this->interruptEnableRegister = interruptEnableRegister;
	}

	void MemoryMap::AttachInterruptFlagRegister(Register8* interruptFlagRegister)
	{
		this->interruptFlagRegister = interruptFlagRegister;
	}

	void MemoryMap::AttachJoypadRegister(Joypad* joypad)
	{
		this->joypad = joypad;
	}

	void MemoryMap::AttachRestrictedMemory(BasicMemory* restrictedMem)
	{
		restrictedMemory = restrictedMem;
	}

	uint8_t MemoryMap::ReadIO(uint16_t address) const
	{
		switch (address)
		{
		case GB_JOYP_ADDRESS:
			return joypad->Read();
		case GB_DIV_ADDRESS:
			return timer->GetDividerRegister();
		case GB_TIMA_ADDRESS:
			return timer->GetTimerCounter();
		case GB_TMA_ADDRESS:
			return timer->GetTimerModulo();
		case GB_TAC_ADDRESS:
			return timer->GetTimerControlRegister();
		case GB_INTERRUPT_FLAG_ADDRESS:
			return interruptFlagRegister->Read();
		case GB_LCDC_ADDRESS:
			return ppu->GetLCDC().Read();
		case GB_LCD_STAT_ADDRESS:
			return ppu->GetLCDStatusRegister().Read();
		case GB_SCY_ADDRESS:
			return ppu->GetSCY().Read();
		case GB_SCX_ADDRESS:
			return ppu->GetSCX().Read();
		case GB_LY_ADDRESS:
			return ppu->GetLY().Read();
		case GB_LYC_ADDRESS:
			return ppu->GetLYC().Read();
		case GB_DMA_TRANSFER_REGISTER_ADDRESS:
			return ppu->GetDMATransferRegister().Read();
		case GB_WY_ADDRESS:
			return ppu->GetWY().Read();
		case GB_WX_ADDRESS:
			return ppu->GetWX().Read();
		default:
			return ioRegisters->Read(Arithmetic::Normalize(address, GB_IO_REGISTERS_START_ADDRESS, GB_IO_REGISTERS_END_ADDRESS));
		}
	}

	void MemoryMap::WriteIO(uint16_t address, uint8_t value)
	{
		switch (address)
		{
		case GB_JOYP_ADDRESS:
			// The upper 2 bits and the lower 4 bits are read-only.
			joypad->Write(WriteWithReadOnlyBits(joypad->Read(), value, 0b00110000));
			break;
		case GB_DIV_ADDRESS:
			timer->WriteToDividerRegister(value);
			break;
		case GB_TIMA_ADDRESS:
			timer->WriteToTimerCounter(value);
			break;
		case GB_TMA_ADDRESS:
			timer->WriteToTimerModulo(value);
			break;
		case GB_TAC_ADDRESS:
			timer->WriteToTimerControlRegister(value);
			break;
		case GB_INTERRUPT_FLAG_ADDRESS:
			interruptFlagRegister->Write(value);
			break;
		case GB_LCDC_ADDRESS:
			ppu->GetLCDC().Write(value);
			break;
		case GB_LCD_STAT_ADDRESS:
			// Lower 3 bits are read only.
			ppu->GetLCDStatusRegister().Write(WriteWithReadOnlyBits(ppu->GetLCDStatusRegister().Read(), value, 0b11111000));
			break;
		case GB_SCY_ADDRESS:
			ppu->GetSCY().Write(value);
			break;
		case GB_SCX_ADDRESS:
			ppu->GetSCX().Write(value);
			break;
		case GB_LY_ADDRESS:
			// LY is read-only.
			break;
		case GB_LYC_ADDRESS:
			ppu->GetLYC().Write(value);
			break;
		case GB_DMA_TRANSFER_REGISTER_ADDRESS:
			ppu->GetDMATransferRegister().Write(value);
			break;
		case GB_WY_ADDRESS:
			ppu->GetWY().Write(value);
			break;
		case GB_WX_ADDRESS:
			ppu->GetWX().Write(value);
			break;
		default:
			ioRegisters->Write(Arithmetic::Normalize(address, GB_IO_REGISTERS_START_ADDRESS, GB_IO_REGISTERS_END_ADDRESS), value);
			break;
		}
	}

	uint8_t MemoryMap::WriteWithReadOnlyBits(uint8_t destination, uint8_t value, uint8_t bitMask)
	{
		return (destination & ~bitMask) | (value & bitMask);
	}
}