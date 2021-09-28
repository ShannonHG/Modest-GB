#include <chrono>
#include <string>
#include <fstream>
#include <filesystem>
#include "Logger.hpp"
#include "AppConfig.hpp"
#include "CommandLineParser.hpp"
#include "Memory/Cartridge.hpp"
#include "Memory/MemoryMap.hpp"
#include "Memory/Memory.hpp"
#include "CPU/CPU.hpp"
#include "Graphics/Display.hpp"
#include "Graphics/PPU.hpp"
#include "Globals.hpp"
#include "CPU/Timer.hpp"
#include "Graphics/TileMapType.hpp"

using namespace SHG;

int main(int argc, char* argv[])
{
	AppConfig config;

	if (TryParseCommandLineArguments(argc, argv, &config))
	{
		Logger::CurrentLogLevel = config.logLevel;

		Logger::WriteWarning("Setting ROM file path to '" + config.romFilePath + "' ");
	}
	else
	{
		return 0;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// TODO: Print SDL error
		Logger::WriteError("[Display] SDL failed to initialize!");
		return 0;
	}

	Logger::WriteWarning("Setting log level to '" + LOG_LEVEL_ENUMS_TO_STRINGS.at(config.logLevel) + "'");

	auto memoryMap = MemoryMap();

	Cartridge cartridge;
	if (!cartridge.LoadFromFile(config.romFilePath))
	{
		Logger::WriteError("Invalid ROM file provided");
		return 0;
	};

	auto vram = Memory(8 * KiB);
	auto wram = Memory(8 * KiB);
	auto oam = Memory(160);
	auto echoRAM = Memory(122368);
	auto restrictedMem = Memory(130464);
	auto ioRegisters = Memory(127);
	auto dmaTransferRegister = DMATransferRegister();
	auto hram = Memory(127);
	auto interruptEnableRegister = Memory(1);
	auto timer = Timer(memoryMap);
	auto serialOutput = Memory(4);

	memoryMap.AssignDeviceToAddressRange(cartridge, 0x0000, 0x7FFF);
	memoryMap.AssignDeviceToAddressRange(vram, 0x8000, 0x9FFF);
	memoryMap.AssignDeviceToAddressRange(cartridge, 0xA000, 0xBFFF);
	memoryMap.AssignDeviceToAddressRange(wram, 0xC000, 0xDFFF);
	memoryMap.AssignDeviceToAddressRange(echoRAM, 0xE000, 0xFDFF);
	memoryMap.AssignDeviceToAddressRange(restrictedMem, 0xFEA0, 0xFEFF);
	memoryMap.AssignDeviceToAddressRange(oam, 0xFE00, 0xFE9F);
	memoryMap.AssignDeviceToAddressRange(serialOutput, 0xFF00, 0xFF03);
	memoryMap.AssignDeviceToAddressRange(timer, 0xFF04, 0xFF07);
	memoryMap.AssignDeviceToAddressRange(ioRegisters, 0xFF08, 0xFF45);
	memoryMap.AssignDeviceToAddressRange(dmaTransferRegister, 0xFF46, 0xFF46);
	memoryMap.AssignDeviceToAddressRange(ioRegisters, 0xFF47, 0xFF7F);
	memoryMap.AssignDeviceToAddressRange(hram, 0xFF80, 0xFFFE);
	memoryMap.AssignDeviceToAddressRange(interruptEnableRegister, 0xFFFF, 0xFFFF);

	// Set default values in memory map
	memoryMap.SetByte(0xFF00, 0xCF);
	memoryMap.SetByte(0xFF01, 0x00);
	memoryMap.SetByte(0xFF02, 0x7E);
	memoryMap.SetByte(0xFF04, 0xAB);
	memoryMap.SetByte(0xFF05, 0x00);
	memoryMap.SetByte(0xFF06, 0x00);
	memoryMap.SetByte(0xFF07, 0xF8);
	memoryMap.SetByte(0xFF0F, 0xE1);
	memoryMap.SetByte(0xFF10, 0x80);
	memoryMap.SetByte(0xFF11, 0xBF);
	memoryMap.SetByte(0xFF12, 0xF3);
	memoryMap.SetByte(0xFF14, 0xBF);
	memoryMap.SetByte(0xFF16, 0x3F);
	memoryMap.SetByte(0xFF17, 0x00);
	memoryMap.SetByte(0xFF18, 0xFF);
	memoryMap.SetByte(0xFF19, 0xBF);
	memoryMap.SetByte(0xFF1A, 0x7F);
	memoryMap.SetByte(0xFF1B, 0xFF);
	memoryMap.SetByte(0xFF1C, 0x9F);
	memoryMap.SetByte(0xFF1D, 0xFF);
	memoryMap.SetByte(0xFF1E, 0xBF);
	memoryMap.SetByte(0xFF20, 0xFF);
	memoryMap.SetByte(0xFF21, 0x00);
	memoryMap.SetByte(0xFF22, 0x00);
	memoryMap.SetByte(0xFF23, 0xBF);
	memoryMap.SetByte(0xFF24, 0x77);
	memoryMap.SetByte(0xFF25, 0xF3);
	memoryMap.SetByte(0xFF26, 0xF1);
	memoryMap.SetByte(0xFF40, 0x91);
	memoryMap.SetByte(0xFF41, 0x85);
	memoryMap.SetByte(0xFF42, 0x00);
	memoryMap.SetByte(0xFF43, 0x00);
	memoryMap.SetByte(0xFF44, 0x00);
	memoryMap.SetByte(0xFF45, 0x00);
	memoryMap.SetByte(0xFF46, 0xFF);
	memoryMap.SetByte(0xFF47, 0xFC);
	memoryMap.SetByte(0xFF48, 0xFF);
	memoryMap.SetByte(0xFF49, 0xFF);
	memoryMap.SetByte(0xFF4A, 0x00);
	memoryMap.SetByte(0xFF4B, 0x00);
	memoryMap.SetByte(0xFF4D, 0xFF);
	memoryMap.SetByte(0xFF4F, 0xFF);
	memoryMap.SetByte(0xFF51, 0xFF);
	memoryMap.SetByte(0xFF52, 0xFF);
	memoryMap.SetByte(0xFF53, 0xFF);
	memoryMap.SetByte(0xFF54, 0xFF);
	memoryMap.SetByte(0xFF55, 0xFF);
	memoryMap.SetByte(0xFF56, 0xFF);
	memoryMap.SetByte(0xFF68, 0xFF);
	memoryMap.SetByte(0xFF69, 0xFF);
	memoryMap.SetByte(0xFF6A, 0xFF);
	memoryMap.SetByte(0xFF6B, 0xFF);
	memoryMap.SetByte(0xFF70, 0xFF);
	memoryMap.SetByte(0xFFFF, 0x00);

	auto processor = CPU(memoryMap);
	processor.ResetToDefaultState();

	std::string windowTitle = "Game Boy Emulator";
	auto display = Display(windowTitle, GB_SCREEN_WIDTH * 3, GB_SCREEN_HEIGHT * 3);
	auto ppu = PPU(display, memoryMap, vram, dmaTransferRegister);

	//auto debugBackgroundMapDisplay = Display("Background Tile Map", TILE_MAP_PIXEL_WIDTH, TILE_MAP_PIXEL_HEIGHT);
	//auto debugBackgroundFramebuffer = Framebuffer(debugBackgroundMapDisplay, TILE_MAP_PIXEL_WIDTH, TILE_MAP_PIXEL_HEIGHT);
	//uint8_t debugBackgroundScanlineX = 0;
	//uint8_t debugBackgroundScanlineY = 0;

	//auto debugWindowMapDisplay = Display("Window Tile Map", TILE_MAP_PIXEL_WIDTH, TILE_MAP_PIXEL_HEIGHT);
	//auto debugWindowFramebuffer = Framebuffer(debugWindowMapDisplay, TILE_MAP_PIXEL_WIDTH, TILE_MAP_PIXEL_HEIGHT);
	//uint8_t debugWindowScanlineX = 0;
	//uint8_t debugWindowScanlineY = 0;

	//uint8_t spriteFramebufferWidth = 116;
	//uint8_t spriteFramebufferHeight = 44;
	//auto debugSpritesDisplay = Display("Sprites", spriteFramebufferWidth * 3, spriteFramebufferHeight * 3);
	//auto debugSpritesFramebuffer = Framebuffer(debugSpritesDisplay, spriteFramebufferWidth, spriteFramebufferHeight);
	//uint8_t debugSpriteIndex = 0;
	//uint8_t debugSpriteScanline = 0;

	auto debugTilesDisplay = Display("Tiles", 256 * 2, 96 * 2);
	auto debugTilesFramebuffer = Framebuffer(debugTilesDisplay, 256, 96);
	uint16_t debugTilesScanlineX = 0;
	uint16_t debugTilesScanlineY = 0;

	bool isRunning = true;
	bool thisFrame = false;

	auto prevTime = std::chrono::system_clock::now();
	double timeSinceTitleUpdate = 0;

	while (isRunning)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				isRunning = false;
				return 0;
			}
		}

		uint32_t duration =  processor.Cycle();
		timer.Update(duration);
		ppu.Cycle(duration);
		/*ppu.DrawTileMap(debugBackgroundMapDisplay, debugBackgroundFramebuffer, debugBackgroundScanlineX, debugBackgroundScanlineY, TileMapType::BackgroundOnly);
		ppu.DrawTileMap(debugWindowMapDisplay, debugWindowFramebuffer, debugWindowScanlineX, debugWindowScanlineY, TileMapType::WindowOnly);
		ppu.DrawSprites(debugSpritesDisplay, debugSpritesFramebuffer, debugSpriteIndex, debugSpriteScanline);*/
		ppu.DrawAllTiles(debugTilesDisplay, debugTilesFramebuffer, debugTilesScanlineX, debugTilesScanlineY);
		processor.HandleInterrupts();

		auto currentTime = std::chrono::system_clock::now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - prevTime);

		double fps = 1.0 / deltaTime.count();
		timeSinceTitleUpdate += deltaTime.count();

		if (timeSinceTitleUpdate >= 1)
		{
			display.SetWindowTitle(windowTitle + " " + std::to_string(fps) + " FPS");
			timeSinceTitleUpdate = 0;
		}

		prevTime = currentTime;
	}

	return 0;
}