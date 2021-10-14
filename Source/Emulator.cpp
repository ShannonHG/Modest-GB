#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Emulator.hpp"
#include "Logger.hpp"
#include "Common/GBSpecs.hpp"
#include "Common/GBMemoryMapAddresses.hpp"

namespace SHG
{
	const uint16_t REGISTER_AF_DEFAULT = 0x01B0;
	const uint16_t REGISTER_BC_DEFAULT = 0x0013;
	const uint16_t REGISTER_DE_DEFAULT = 0x00D8;
	const uint16_t REGISTER_HL_DEFAULT = 0x014D;
	const uint16_t STACK_POINTER_DEFAULT = 0xFFFE;
	const uint16_t PROGRAM_COUNTER_DEFAULT = 0x0100;

	const double FRAME_RENDER_INTERVAL = 1 / 60.0;

	bool Emulator::Start(const std::string& romFilePath)
	{
		// TODO: Temporary
		//Logger::IsSystemEventLoggingEnabled = true;

		Logger::RegisterLogEntryCallback(std::bind(&Emulator::AddLogEntry, this, std::placeholders::_1));

		if (!cartridge.LoadFromFile(romFilePath))
		{
			Logger::WriteError("Invalid ROM file provided");
			return false;
		};

		// TODO: Check if successful
		window.Initialize();

		InitializeCPU();
		ppu.InitializeFramebuffer(window.GetSDLWindow());
		InitializeMemoryMap();

		isRunning = true;
		double timeSinceLastFrame = 0;

		double timeSinceLastCycleCount = 0;
		uint32_t elaspedCycles = 0;

		auto prevTime = std::chrono::system_clock::now();

		while (isRunning)
		{
			// TODO: Revisit
			window.PollEvents(std::bind(&Emulator::OnQuit, this));

			if (!isRunning)
				return true;

			// TODO: Implement pausing and stepping functionality.
			if (!isPaused || isStepRequested)
			{
				uint32_t cycles = processor.Step();
				elaspedCycles += cycles;

				timer.Step(cycles);
				ppu.Step(cycles);

				// TODO: Only populate these debug framebuffers when the respective windows are open.
				ppu.DebugDrawTiles();
				ppu.DebugDrawSprites();
				ppu.DebugDrawBackgroundTileMap();
				ppu.DebugDrawWindowTileMap();

				processor.HandleInterrupts();

				isStepRequested = false;
			}

			auto currentTime = std::chrono::system_clock::now();
			auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - prevTime).count();
			prevTime = currentTime;

			timeSinceLastFrame += deltaTime;
			timeSinceLastCycleCount += deltaTime;

			if (timeSinceLastCycleCount >= 1)
			{
				cyclesPerSecond = elaspedCycles;
				elaspedCycles = 0;
				timeSinceLastCycleCount = 0;
			}

			// TODO: Save leftover time
			if (timeSinceLastFrame >= FRAME_RENDER_INTERVAL)
			{
				window.Render(memoryMap, ppu, processor, cyclesPerSecond, logEntries);

				timeSinceLastFrame = 0;
			}
		}

		return true;
	}

	void Emulator::InitializeCPU()
	{
		processor.GetRegisterAF().SetData(REGISTER_AF_DEFAULT);
		processor.GetRegisterBC().SetData(REGISTER_BC_DEFAULT);
		processor.GetRegisterDE().SetData(REGISTER_DE_DEFAULT);
		processor.GetRegisterHL().SetData(REGISTER_HL_DEFAULT);
		processor.GetStackPointer().SetData(STACK_POINTER_DEFAULT);
		processor.GetProgramCounter().SetData(PROGRAM_COUNTER_DEFAULT);
	}

	void Emulator::InitializeMemoryMap()
	{
		// TODO: Use constants for addresses instead of hardcoded values

		memoryMap.AssignDeviceToAddressRange(cartridge, 0x0000, 0x7FFF);
		memoryMap.AssignDeviceToAddressRange(vram, 0x8000, 0x9FFF);
		memoryMap.AssignDeviceToAddressRange(cartridge, 0xA000, 0xBFFF);
		memoryMap.AssignDeviceToAddressRange(wram, 0xC000, 0xDFFF);
		memoryMap.AssignDeviceToAddressRange(echoRam, 0xE000, 0xFDFF);
		memoryMap.AssignDeviceToAddressRange(restrictedMemory, 0xFEA0, 0xFEFF);
		memoryMap.AssignDeviceToAddressRange(oam, 0xFE00, 0xFE9F);
		memoryMap.AssignDeviceToAddressRange(serialOutputRegister, 0xFF01, 0xFF03);
		memoryMap.AssignDeviceToAddressRange(timer, 0xFF04, 0xFF07);
		memoryMap.AssignDeviceToAddressRange(hram, 0xFF80, 0xFFFE);
		memoryMap.AssignDeviceToAddressRange(interruptFlagRegister, 0xFF0F, 0xFF0F);
		memoryMap.AssignDeviceToAddressRange(interruptEnableRegister, 0xFFFF, 0xFFFF);
		memoryMap.AssignDeviceToAddressRange(joypadRegister, 0xFF00, 0xFF00);

		memoryMap.AssignDeviceToAddressRange(ioRegisters, 0xFF08, 0xFF0E);
		memoryMap.AssignDeviceToAddressRange(ioRegisters, 0xFF10, 0xFF39);
		memoryMap.AssignDeviceToAddressRange(ioRegisters, 0xFF4C, 0xFF7F);
		memoryMap.AssignDeviceToAddressRange(ioRegisters, 0xFF47, 0xFF49);

		memoryMap.AssignDeviceToAddressRange(*ppu.GetLCDC(), GB_LCD_CONTROL_REGISTER_ADDRESS, GB_LCD_CONTROL_REGISTER_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetLCDStatus(), GB_LCD_STATUS_REGISTER_ADDRESS, GB_LCD_STATUS_REGISTER_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetSCY(), GB_SCY_ADDRESS, GB_SCY_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetSCX(), GB_SCX_ADDRESS, GB_SCX_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetLY(), GB_LY_ADDRESS, GB_LY_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetLYC(), GB_LYC_ADDRESS, GB_LYC_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetWY(), GB_WY_ADDRESS, GB_WY_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetWX(), GB_WX_ADDRESS, GB_WX_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(*ppu.GetDMATransferRegister(), GB_DMA_TRANSFER_REGISTER_ADDRESS, GB_DMA_TRANSFER_REGISTER_ADDRESS);

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

		// Set the lower nibble of the joypad register to read-only
		memoryMap.SetReadonlyBitMask(0xFF00, 0x0F);

		// Set LYC=LY and Mode flag bits to read-only
		memoryMap.SetReadonlyBitMask(0xFF41, 0b0000111);

		// Set LY to read-only
		memoryMap.SetReadonlyBitMask(0xFF44, 0xFF);
	}

	void Emulator::AddLogEntry(std::string logEntry)
	{
		logEntries += logEntry + '\n';
	}

	void Emulator::OnQuit()
	{
		isRunning = false;
	}
}