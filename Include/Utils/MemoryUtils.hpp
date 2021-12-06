#pragma once
#include <cstdint>

namespace SHG
{
	const uint16_t KiB = 1024;
	const uint32_t MiB = pow(1024, 2);
	const uint64_t GiB = pow(1024, 3);

	const uint16_t GB_ROM_BANK_00_START_ADDRESS = 0x0000;
	const uint16_t GB_ROM_BANK_00_END_ADDRESS = 0x3FFF;

	const uint16_t GB_SWITCHABLE_ROM_BANK_START_ADDRESS = 0x4000;
	const uint16_t GB_SWITCHABLE_ROM_BANK_END_ADDRESS = 0x7FFF;

	const uint16_t GB_VRAM_START_ADDRESS = 0x8000;
	const uint16_t GB_VRAM_END_ADDRESS = 0x9FFF;

	const uint16_t GB_EXTERNAL_RAM_START_ADDRESS = 0xA000;
	const uint16_t GB_EXTERNAL_RAM_END_ADDRESS = 0xBFFF;

	const uint16_t GB_WORK_RAM_START_ADDRESS = 0xC000;
	const uint16_t GB_WORK_RAM_END_ADDRESS = 0xDFFF;

	const uint16_t GB_ECHO_RAM_START_ADDRESS = 0xE000;
	const uint16_t GB_ECHO_RAM_END_ADDRESS = 0xFDFF;

	const uint16_t GB_OAM_START_ADDRESS = 0xFE00;
	const uint16_t GB_OAM_END_ADDRESS = 0xFE9F;

	const uint16_t GB_UNUSABLE_MEMORY_START_ADDRESS = 0xFEA0;
	const uint16_t GB_UNUSABLE_MEMORY_END_ADDRESS = 0xFEFF;

	const uint16_t GB_IO_REGISTERS_START_ADDRESS = 0xFF00;
	const uint16_t GB_IO_REGISTERS_END_ADDRESS = 0xFF7F;

	const uint16_t GB_JOYP_ADDRESS = 0xFF00;
	const uint16_t GB_SERIAL_TRANSFER_DATA_ADDRESS = 0xFF01;
	const uint16_t GB_SERIAL_TRANSFER_CONTROL_ADDRESS = 0xFF02;

	const uint16_t GB_DIV_ADDRESS = 0xFF04;
	const uint16_t GB_TIMA_ADDRESS = 0xFF05;
	const uint16_t GB_TMA_ADDRESS = 0xFF06;
	const uint16_t GB_TAC_ADDRESS = 0xFF07;

	const uint16_t GB_LCDC_ADDRESS = 0xFF40;
	const uint16_t GB_LCD_STAT_ADDRESS = 0xFF41;
	const uint16_t GB_SCY_ADDRESS = 0xFF42;
	const uint16_t GB_SCX_ADDRESS = 0xFF43;

	// LY is the current scanline, and can hold any value from 0 to 153. 
	// Values 144 to 153 indicate the VBlank period.
	const uint16_t GB_LY_ADDRESS = 0xFF44;

	// LYC is constantly compared to LY. When LY and LYC are equal, the "LYC=LY" flag
	// is enabled in the STAT register and a STAT interrupt is requested.
	const uint16_t GB_LYC_ADDRESS = 0xFF45;

	const uint16_t GB_BACKGROUND_PALETTE_ADDRESS = 0xFF47;
	const uint16_t GB_DMA_TRANSFER_REGISTER_ADDRESS = 0xFF46;
	const uint16_t GB_SPRITE_PALETTE_0_ADDRESS = 0xFF48;
	const uint16_t GB_SPRITE_PALETTE_1_ADDRESS = 0xFF49;
	const uint16_t GB_WY_ADDRESS = 0xFF4A;
	const uint16_t GB_WX_ADDRESS = 0xFF4B;

	const uint16_t GB_HIGH_RAM_START_ADDRESS = 0xFF80;
	const uint16_t GB_HIGH_RAM_END_ADDRESS = 0xFFFE;

	const uint16_t GB_INTERRUPT_FLAG_ADDRESS = 0xFF0F;
	const uint16_t GB_INTERRUPT_ENABLE_ADDRESS = 0xFFFF;
}