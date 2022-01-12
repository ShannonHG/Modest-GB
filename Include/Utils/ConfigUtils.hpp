#pragma once
#include "Audio/APU.hpp"
#include "Graphics/PPU.hpp"
#include "Input/Joypad.hpp"
#include "EmulatorWindow.hpp"

namespace SHG::Config
{
	bool SaveConfiguration(const std::string& configFilePath, EmulatorWindow& window, const APU& apu, const PPU& ppu, const Joypad& joypad, const Cartridge& cartride);
	bool LoadConfiguration(const std::string& configFilePath, EmulatorWindow& window, APU& apu, PPU& ppu, Joypad& joypad, Cartridge& cartridge);
}