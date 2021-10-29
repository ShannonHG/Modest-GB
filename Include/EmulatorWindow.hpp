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
		bool Initialize();
		SDL_Window* GetSDLWindow();
		void Render(MemoryMap& memoryMap, PPU& ppu, CPU& processor, uint32_t cyclesPerSecond, std::string& logs);
	
		bool shouldRenderCPUWindow = false;
		bool shouldRenderIOWindow = false;
		bool shouldRenderLogWindow = false;
		bool shouldRenderTilesWindow = false;
		bool shouldRenderSpritesWindow = false;
		bool shouldRenderBackgroundTileMapWindow = false;
		bool shouldRenderWindowTileMapWindow = false;
		bool shouldRenderVideoRegistersWindow = false;

		void RegisterFileSelectionCallback(FileSelectionEvent callback);
		void RegisterPauseButtonCallback(SimpleEvent callback);
		void RegisterStepButtonCallback(SimpleEvent callback);
		void RegisterClearButtonCallback(SimpleEvent callback);
		void RegisterQuitButtonCallback(SimpleEvent callback);

		void SetPauseButtonLabel(const std::string& label);

		bool IsTraceEnabled();

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
		void RenderMainWindow();
		void RenderGameView(PPU& ppu);
		void RenderWindowWithFramebuffer(const std::string& title, Framebuffer& framebuffer, bool* isOpen = nullptr);
		void RenderCPUWindow(CPU& processor, uint32_t cyclesPerSecond);
		void RenderIOWindow(MemoryMap& memoryMap, CPU& processor);
		void RenderTilesWindow(PPU& ppu);
		void RenderSpritesWindow(PPU& ppu);
		void RenderBackgroundTileMapWindow(PPU& ppu);
		void RenderWindowTileMapWindow(PPU& ppu);
		void RenderVideoRegistersWindow(PPU& ppu);
		void RenderLogWindow(std::string& logEntries);
		void EndFrame();
	};
}