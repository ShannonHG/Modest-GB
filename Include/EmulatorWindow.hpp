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
		bool shouldAutoScrollLogsToBottom = false;

		bool Initialize();
		void Quit();
		SDL_Window* GetSDLWindow();
		void Render(const MemoryMap& memoryMap, PPU& ppu, const CPU& processor, APU& apu, Joypad& joypad, InputManager& inputManager, Cartridge& cartridge, const Timer& timer, uint32_t cyclesPerSecond, const std::vector<std::string>& logEntries);
	
		void RegisterFileSelectionCallback(FileSelectionCallback callback);
		void RegisterPauseButtonCallback(SimpleCallback callback);
		void RegisterStepButtonCallback(SimpleCallback callback);
		void RegisterClearButtonCallback(SimpleCallback callback);
		void RegisterQuitButtonCallback(SimpleCallback callback);

		void SetPauseButtonLabel(const std::string& label);

		bool IsMaximized();
		void SetMaximizedValue(bool value, bool modifyWindow);
		void Show();

	private:
		SDL_Window* sdlWindow = nullptr;
		SDL_Renderer* sdlRenderer = nullptr;

		FileSelectionCallback romFileSelectionCallback;
		SimpleCallback pauseButtonPressedCallback;
		SimpleCallback stepButtonPressedCallback;
		SimpleCallback clearButtonPressedCallback;
		SimpleCallback quitButtonPressedCallback;

		std::string pauseButtonLabel = "Pause";
		bool isMaximized = false;

		ImGuiID dockspaceID = 0;

		void StartFrame();
		void ClearScreen();
		void EndFrame();
		void RenderMainWindow();
		void RenderGameView(const PPU& ppu);
		void RenderWindowWithFramebuffer(const std::string& title, const Framebuffer& framebuffer, bool* isOpen = nullptr);
		void RenderCPUDebugWindow(const CPU& processor, const MemoryMap& memoryMap, uint32_t cyclesPerSecond);
		void RenderSoundDebugWindow(APU& apu);
		void RenderJoypadDebugWindow(const Joypad& joypad);
		void RenderVideoRegistersDebugWindow(const PPU& ppu);
		void RenderTilesDebugWindow(const PPU& ppu);
		void RenderSpritesDebugWindow(const PPU& ppu);
		void RenderBackgroundTileMapDebugWindow(const PPU& ppu);
		void RenderWindowTileMapDebugWindow(const PPU& ppu);
		void RenderTimerDebugWindow(const Timer& timer);
		void RenderLogWindow(const std::vector<std::string>& logEntries);
		
		void RenderSettingsWindow(PPU& ppu, APU& apu, Joypad& joypad, InputManager& inputManager, Cartridge& cartridge);
		void RenderSettingsWindowSelectableItem(const std::string& label, int selectableID, int& selectedWindowID);

		void RenderVideoSettingsWindow(PPU& ppu);
		void RenderAudioSettingsWindow(APU& apu);
		void RenderControllerAndKeyboardSettingsWindow(Joypad& joypad, InputManager&);
		void RenderControllerButtonComboBox(Joypad& joypad, GBButton gbButton, int row, float width);
		void RenderKeyCodeComboBox(Joypad& joypad, GBButton gbButton, int row, float width);
		void RenderColorPaletteButton(PPU& ppu, const std::string& label, uint16_t paletteAddress, uint8_t colorIndex, uint16_t& outPaletteAddress, uint8_t& outColorIndex, std::string& outLabel, bool& isColorPickerOpened);
		void RenderSavedDataSettingsWindow(Cartridge& cartridge);

		void RenderRegister8(const std::string& label, uint8_t data, float labelCursorPos, float dataSpacing);
		void RenderRegister16(const std::string& label, uint16_t data, float labelCursorPos, float dataSpacing);

		bool BeginSelectable(const std::string& label, bool isSelected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
		void EndSelectable(bool isSelected);

		bool BeginWindow(const std::string& title, bool* isOpen, ImGuiWindowFlags flags);
		void EndWindow();

		void ApplyTheme();

		std::string GetPathFromFileBrowser(const std::string& filterFriendlyName, const std::string& filters);

		ImVec4 ConvertColorToImVec4(Color& color) const;
		Color ConvertImVec4ToColor(ImVec4& vec) const;
	};
}