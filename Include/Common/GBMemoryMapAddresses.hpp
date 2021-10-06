#pragma once
#include <cstdint>

namespace SHG
{
	// Graphics

	const uint16_t GB_SCY_ADDRESS = 0xFF42;
	const uint16_t GB_SCX_ADDRESS = 0xFF43;
	const uint16_t GB_LY_ADDRESS = 0xFF44;
	const uint16_t GB_LYC_ADDRESS = 0xFF45;
	const uint16_t GB_WY_ADDRESS = 0xFF4A;
	const uint16_t GB_WX_ADDRESS = 0xFF4B;
	const uint16_t GB_LCD_CONTROL_REGISTER_ADDRESS = 0xFF40;
	const uint16_t GB_LCD_STATUS_REGISTER_ADDRESS = 0xFF41;
	const uint16_t GB_DMA_TRANSFER_REGISTER_ADDRESS = 0xFF46;
}