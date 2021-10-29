#pragma once
#include <cstdint>

namespace SHG
{
	// Graphics

	const uint16_t GB_SCY_ADDRESS = 0xFF42;
	const uint16_t GB_SCX_ADDRESS = 0xFF43;

	// LY is the current scanline, and can hold any value from 0 to 153. 
	// Values 144 to 153 indicate the VBlank period.
	const uint16_t GB_LY_ADDRESS = 0xFF44;

	// LYC is constantly compared to LY. When LY and LYC are equal, the "LYC=LY" flag
	// is enabled in the STAT register and a STAT interrupt is requested.
	const uint16_t GB_LYC_ADDRESS = 0xFF45;
	const uint16_t GB_WY_ADDRESS = 0xFF4A;
	const uint16_t GB_WX_ADDRESS = 0xFF4B;
	const uint16_t GB_LCD_CONTROL_REGISTER_ADDRESS = 0xFF40;
	const uint16_t GB_LCD_STATUS_REGISTER_ADDRESS = 0xFF41;
	const uint16_t GB_DMA_TRANSFER_REGISTER_ADDRESS = 0xFF46;

	// IO

	const uint16_t GB_JOYP_ADDRESS = 0xFF00;
	const uint16_t GB_SB_ADDRESS = 0xFF01;
	const uint16_t GB_SC_ADDRESS = 0xFF02;

	// Timer

	const uint16_t GB_DIV_ADDRESS = 0xFF04;
	const uint16_t GB_TIMA_ADDRESS = 0xFF05;
	const uint16_t GB_TMA_ADDRESS = 0xFF06;
	const uint16_t GB_TAC_ADDRESS = 0xFF07;

	// Interrupts

	const uint16_t GB_INTERRUPT_ENABLE_ADDRESS = 0xFFFF;
	const uint16_t GB_INTERRUPT_FLAG_ADDRESS = 0xFF0F;


}