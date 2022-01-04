#pragma once
#include <functional>
#include "SDL.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Graphics/PPU.hpp"
#include "CPU/CPU.hpp"
#include "Memory/MemoryMap.hpp"
#include "Utils/Events.hpp"

namespace SHG
{
	class EmulatorWindow
	{
	public:
		bool shouldRenderCPUDebugWindow = false;
		bool shouldRenderSoundDebugWindow = false;
		bool shouldRenderJoypadDebugWindow = false;
		bool shouldRenderVideoRegistersDebugWindow = false;
		bool shouldRenderTilesDebugWindow = false;
		bool shouldRenderSpritesDebugWindow = false;
		bool shouldRenderBackgroundTileMapDebugWindow = false;
		bool shouldRenderWindowTileMapDebugWindow = false;
		bool shouldRenderTimerDebugWindow = false;
		bool shouldRenderLogWindow = false;
		bool shouldRenderSettingsWindow = false;

		bool Initialize();
		SDL_Window* GetSDLWindow();
		void Render(MemoryMap& memoryMap, PPU& ppu, CPU& processor, APU& apu, Joypad& joypad, Timer& timer, uint32_t cyclesPerSecond, std::string& logEntries);
	
		void RegisterFileSelectionCallback(FileSelectionEvent callback);
		void RegisterPauseButtonCallback(SimpleEvent callback);
		void RegisterStepButtonCallback(SimpleEvent callback);
		void RegisterClearButtonCallback(SimpleEvent callback);
		void RegisterQuitButtonCallback(SimpleEvent callback);

		void SetPauseButtonLabel(const std::string& label);

		bool IsTraceEnabled();
		int GetWidth();
		int GetHeight();
		void SetSize(int width, int height);

		int GetX();
		int GetY();
		void SetPosition(int x, int y);

	private:
		SDL_Window* sdlWindow = nullptr;
		SDL_Renderer* sdlRenderer = nullptr;

		FileSelectionEvent romFileSelectionCallback;
		SimpleEvent pauseButtonPressedCallback;
		SimpleEvent stepButtonPressedCallback;
		SimpleEvent clearButtonPressedCallback;
		SimpleEvent quitButtonPressedCallback;

		std::string pauseButtonLabel = "Pause";

		bool isTraceEnabled = false;

		ImGuiID dockspaceID = 0;

		void StartFrame();
		void ClearScreen();
		void EndFrame();
		void RenderMainWindow();
		void RenderGameView(PPU& ppu);
		void RenderWindowWithFramebuffer(const std::string& title, Framebuffer& framebuffer, bool* isOpen = nullptr);
		void RenderCPUDebugWindow(CPU& processor, MemoryMap& memoryMap, uint32_t cyclesPerSecond);
		void RenderSoundDebugWindow(APU& apu);
		void RenderJoypadDebugWindow(Joypad& joypad);
		void RenderVideoRegistersDebugWindow(PPU& ppu);
		void RenderTilesDebugWindow(PPU& ppu);
		void RenderSpritesDebugWindow(PPU& ppu);
		void RenderBackgroundTileMapDebugWindow(PPU& ppu);
		void RenderWindowTileMapDebugWindow(PPU& ppu);
		void RenderTimerDebugWindow(Timer& timer);
		void RenderLogWindow(std::string& logEntries);
		
		void RenderSettingsWindow(PPU& ppu, APU& apu, Joypad& joypad);
		void RenderVideoSettingsWindow(PPU& ppu);
		void RenderAudioSettingsWindow(APU& apu);
		void RenderControllerAndKeyboardSettingsWindow(Joypad& joypad);

		void RenderColorPaletteButton(const PPU& ppu, const std::string& label, uint16_t paletteAddress, uint8_t colorIndex, uint16_t* outPaletteAddress, uint8_t* outColorIndex, std::string* outLabel, bool* isColorPickerOpened);

		ImVec4 ConvertColorToImVec4(const Color& color) const;
		Color ConvertImVec4ToColor(const ImVec4 vec) const;
	};
}