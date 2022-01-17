#include "nfd.h"
#include "EmulatorWindow.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/MemoryUtils.hpp"

namespace SHG
{
	const char* DEFAULT_WINDOW_TITLE = "Game Boy Emulator";
	const uint16_t DEFAULT_SDL_WINDOW_WIDTH = 1024;
	const uint16_t DEFAULT_SDL_WINDOW_HEIGHT = 576;
	const uint16_t MINIMUM_SDL_WINDOW_WIDTH = 256;
	const uint16_t MINIMUM_SDL_WINDOW_HEIGHT = 144;

	const uint8_t SETTINGS_VIDEO_WINDOW_ID = 0;
	const uint8_t SETTINGS_AUDIO_WINDOW_ID = 1;
	const uint8_t SETTINGS_CONTROLLER_AND_KEYBOARD_WINDOW_ID = 2;
	const uint8_t SETTINGS_SAVED_DATA_WINDOW_ID = 3;

	const uint8_t BACKGROUND_AND_WINDOW_PALETTE_TINT_ID_0 = 0;
	const uint8_t BACKGROUND_AND_WINDOW_PALETTE_TINT_ID_1 = 1;
	const uint8_t BACKGROUND_AND_WINDOW_PALETTE_TINT_ID_2 = 2;
	const uint8_t BACKGROUND_AND_WINDOW_PALETTE_TINT_ID_3 = 3;

	const uint8_t SPRITE_1_PALETTE_TINT_ID_0 = 4;
	const uint8_t SPRITE_1_PALETTE_TINT_ID_1 = 5;
	const uint8_t SPRITE_1_PALETTE_TINT_ID_2 = 6;
	const uint8_t SPRITE_1_PALETTE_TINT_ID_3 = 7;

	const uint8_t SPRITE_2_PALETTE_TINT_ID_0 = 8;
	const uint8_t SPRITE_2_PALETTE_TINT_ID_1 = 9;
	const uint8_t SPRITE_2_PALETTE_TINT_ID_2 = 10;
	const uint8_t SPRITE_2_PALETTE_TINT_ID_3 = 11;

	const float MAX_VOLUME = 100.0f;

	const std::map<SavedDataSearchType, std::string> SAVED_DATA_SEARCH_TYPE_STRINGS
	{
		{ SavedDataSearchType::ROM_DIRECTORY, "ROM Directory" },
		{ SavedDataSearchType::EMULATOR_DIRECTORY, "Emulator Directory" },
	};

	const ImGuiWindowFlags MAIN_WINDOW_FLAGS = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoSavedSettings;

	const ImGuiWindowFlags GENERAL_WINDOW_FLAGS = ImGuiWindowFlags_NoCollapse;

	bool EmulatorWindow::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		{
			Logger::WriteError("SDL failed to initialize. Error: " + std::string(SDL_GetError()));
			return false;
		}

		uint32_t flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;

		sdlWindow = SDL_CreateWindow(DEFAULT_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_SDL_WINDOW_WIDTH, DEFAULT_SDL_WINDOW_HEIGHT, flags);

		if (sdlWindow == nullptr)
		{
			Logger::WriteError("Failed to create SDL window. Error: " + std::string(SDL_GetError()));
			return false;
		}

		SDL_SetWindowMinimumSize(sdlWindow, MINIMUM_SDL_WINDOW_WIDTH, MINIMUM_SDL_WINDOW_HEIGHT);

		sdlRenderer = SDL_CreateRenderer(sdlWindow, 0, 0);

		if (sdlRenderer == nullptr)
		{
			Logger::WriteError("Failed to create SDL renderer. Error: " + std::string(SDL_GetError()));
			return false;
		}

		IMGUI_CHECKVERSION();
		if (ImGui::CreateContext() == nullptr)
		{
			Logger::WriteError("Failed to create ImGui context.");
			return false;
		}

		ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow);
		ImGui_ImplSDLRenderer_Init(sdlRenderer);

		if (NFD_Init() != NFD_OKAY)
		{
			Logger::WriteError("Failed to initialize NFD.");
			return false;
		}

		ApplyTheme();

		return true;
	}

	void EmulatorWindow::Quit()
	{
		NFD_Quit();
		ImGui::DestroyContext();
		SDL_Quit();
	}

	void EmulatorWindow::Show()
	{
		SDL_ShowWindow(sdlWindow);
	}

	bool EmulatorWindow::IsMaximized()
	{
		return isMaximized;
	}

	void EmulatorWindow::SetMaximizedValue(bool value, bool modifyWindow)
	{
		isMaximized = value;
		if (modifyWindow)
		{
			if (isMaximized)
				SDL_MaximizeWindow(sdlWindow);
			else
				SDL_RestoreWindow(sdlWindow);
		}
	}

	SDL_Window* EmulatorWindow::GetSDLWindow()
	{
		return sdlWindow;
	}

	void EmulatorWindow::Render(const MemoryMap& memoryMap, PPU& ppu, const CPU& processor, APU& apu, Joypad& joypad, InputManager& inputManager, Cartridge& cartridge, const Timer& timer, uint32_t cyclesPerSecond, std::string& logEntries)
	{
		StartFrame();
		ClearScreen();

		RenderMainWindow();
		RenderGameView(ppu);

		if (shouldRenderCPUDebugWindow)
			RenderCPUDebugWindow(processor, memoryMap, cyclesPerSecond);

		if (shouldRenderSoundDebugWindow)
			RenderSoundDebugWindow(apu);

		if (shouldRenderJoypadDebugWindow)
			RenderJoypadDebugWindow(joypad);

		if (shouldRenderVideoRegistersDebugWindow)
			RenderVideoRegistersDebugWindow(ppu);

		if (shouldRenderTilesDebugWindow)
			RenderTilesDebugWindow(ppu);

		if (shouldRenderSpritesDebugWindow)
			RenderSpritesDebugWindow(ppu);

		if (shouldRenderBackgroundTileMapDebugWindow)
			RenderBackgroundTileMapDebugWindow(ppu);

		if (shouldRenderWindowTileMapDebugWindow)
			RenderWindowTileMapDebugWindow(ppu);

		if (shouldRenderTimerDebugWindow)
			RenderTimerDebugWindow(timer);

		if (shouldRenderLogWindow)
			RenderLogWindow(logEntries);

		if (shouldRenderSettingsWindow)
			RenderSettingsWindow(ppu, apu, joypad, inputManager, cartridge);

		EndFrame();
	}

	void EmulatorWindow::StartFrame()
	{
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame(sdlWindow);
		ImGui::NewFrame();
	}

	void EmulatorWindow::ClearScreen()
	{
		SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
		SDL_RenderClear(sdlRenderer);
	}

	void EmulatorWindow::RegisterFileSelectionCallback(FileSelectionCallback callback)
	{
		romFileSelectionCallback = callback;
	}

	void EmulatorWindow::RegisterPauseButtonCallback(SimpleCallback callback)
	{
		pauseButtonPressedCallback = callback;
	}

	void EmulatorWindow::RegisterStepButtonCallback(SimpleCallback callback)
	{
		stepButtonPressedCallback = callback;
	}

	void EmulatorWindow::RegisterClearButtonCallback(SimpleCallback callback)
	{
		clearButtonPressedCallback = callback;
	}

	void EmulatorWindow::RegisterQuitButtonCallback(SimpleCallback callback)
	{
		quitButtonPressedCallback = callback;
	}

	void EmulatorWindow::RenderMainWindow()
	{
		// Make the main window have the same size and position as the main viewport.
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		// Create a window, with a menu bar, that can be used as dock space for other windows.
		if (ImGui::Begin("Main", nullptr, MAIN_WINDOW_FLAGS))
		{
			// Main menu bar
			if (ImGui::BeginMenuBar())
			{
				auto& colors = ImGui::GetStyle().Colors;
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors[ImGuiCol_TabActive]);

				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Load ROM"))
					{
						std::string path = GetPathFromFileBrowser("Game Boy ROMs", "gb,rom");

						if (!path.empty())
							romFileSelectionCallback(path);
					}

					ImGui::Spacing();
					if (ImGui::MenuItem("Settings"))
						shouldRenderSettingsWindow = true;

					ImGui::EndMenu();
				}

				// Menu that enables the user to open debugging tools/windows.
				if (ImGui::BeginMenu("Debug"))
				{
					if (ImGui::BeginMenu("Video"))
					{
						if (ImGui::MenuItem("Registers"))
							shouldRenderVideoRegistersDebugWindow = true;

						ImGui::Spacing();
						if (ImGui::MenuItem("Tiles"))
							shouldRenderTilesDebugWindow = true;

						ImGui::Spacing();
						if (ImGui::MenuItem("Sprites"))
							shouldRenderSpritesDebugWindow = true;

						ImGui::Spacing();
						if (ImGui::MenuItem("Background Tile Map"))
							shouldRenderBackgroundTileMapDebugWindow = true;

						ImGui::Spacing();
						if (ImGui::MenuItem("Window Tile Map"))
							shouldRenderWindowTileMapDebugWindow = true;

						ImGui::EndMenu();
					}

					ImGui::Spacing();
					if (ImGui::MenuItem("CPU"))
						shouldRenderCPUDebugWindow = true;

					ImGui::Spacing();
					if (ImGui::MenuItem("Sound"))
						shouldRenderSoundDebugWindow = true;

					ImGui::Spacing();
					if (ImGui::MenuItem("Joypad"))
						shouldRenderJoypadDebugWindow = true;

					ImGui::Spacing();
					if (ImGui::MenuItem("Timer"))
						shouldRenderTimerDebugWindow = true;

					ImGui::Spacing();
					if (ImGui::MenuItem("Logs"))
						shouldRenderLogWindow = true;

					ImGui::EndMenu();
				}

				ImGui::PopStyleColor();
				ImGui::EndMenuBar();
			}
		}

		dockspaceID = ImGui::GetID("Dockspace");
		ImGui::DockSpace(dockspaceID, ImVec2(0, 0), ImGuiDockNodeFlags_None);
		ImGui::End();
	}

	bool EmulatorWindow::BeginWindow(const std::string& title, bool* isOpen, ImGuiWindowFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
		return ImGui::Begin(title.c_str(), isOpen, flags);
	}

	void EmulatorWindow::EndWindow()
	{
		ImGui::PopStyleColor();
		ImGui::End();
	}

	// TODO: Remove scrollbar
	void EmulatorWindow::RenderGameView(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window containing the currently loaded game, if any.
		RenderWindowWithFramebuffer("Game", ppu.GetPrimaryFramebuffer());
	}

	void EmulatorWindow::RenderWindowWithFramebuffer(const std::string& title, const Framebuffer& framebuffer, bool* isOpen)
	{
		if (BeginWindow(title.c_str(), isOpen, GENERAL_WINDOW_FLAGS | ImGuiWindowFlags_NoScrollbar))
		{
			ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();
			ImVec2 imageSize = contentRegionSize;

			// Preserve the aspect ratio of the frameBuffer. 
			if (contentRegionSize.x / contentRegionSize.y < framebuffer.GetAspectRatio())
			{
				imageSize.x = contentRegionSize.x;
				imageSize.y = imageSize.x / framebuffer.GetAspectRatio();
			}
			else
			{
				imageSize.y = contentRegionSize.y;
				imageSize.x = imageSize.y * framebuffer.GetAspectRatio();
			}

			ImVec2 windowSize = ImGui::GetWindowSize();

			// Position the game view in the center of the window.
			ImGui::SetCursorPosX(((contentRegionSize.x - imageSize.x) + (windowSize.x - contentRegionSize.x)) * 0.5f);
			ImGui::SetCursorPosY(((contentRegionSize.y - imageSize.y) + (windowSize.y - contentRegionSize.y)) * 0.5f + 10);

			ImGui::Image((void*)framebuffer.GetTexture(), imageSize);
		}

		EndWindow();
	}

	void EmulatorWindow::RenderCPUDebugWindow(const CPU& processor, const MemoryMap& memoryMap, uint32_t cyclesPerSecond)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing CPU related information (clock speed, registers, etc.).
		if (BeginWindow("CPU", &shouldRenderCPUDebugWindow, GENERAL_WINDOW_FLAGS))
		{
			ImGui::Text("Performance");
			ImGui::Separator();
			ImGui::Text(("Clock speed: " + std::to_string(cyclesPerSecond / 1000000.0) + " MHz").c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			static bool isPaused = false;
			if (ImGui::Button(pauseButtonLabel.c_str()))
			{
				isPaused = !isPaused;
				pauseButtonPressedCallback();
			}

			ImGui::SameLine();

			// "Stepping" only works when the emulator is paused.
			if (isPaused)
			{
				if (ImGui::Button("Step"))
					stepButtonPressedCallback();
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Registers");
			ImGui::Separator();

			// Print register values 
			ImGui::Text(("A: " + GetHexString8(processor.ReadRegisterA())).c_str());
			ImGui::SameLine();
			ImGui::Text(("F: " + GetHexString8(processor.ReadRegisterF())).c_str());

			ImGui::Text(("B: " + GetHexString8(processor.ReadRegisterB())).c_str());
			ImGui::SameLine();
			ImGui::Text(("C: " + GetHexString8(processor.ReadRegisterC())).c_str());

			ImGui::Text(("D: " + GetHexString8(processor.ReadRegisterD())).c_str());
			ImGui::SameLine();
			ImGui::Text(("E: " + GetHexString8(processor.ReadRegisterE())).c_str());

			ImGui::Text(("H: " + GetHexString8(processor.ReadRegisterH())).c_str());
			ImGui::SameLine();
			ImGui::Text(("L: " + GetHexString8(processor.ReadRegisterL())).c_str());

			ImGui::Text(("PC: " + GetHexString16(processor.ReadProgramCounter())).c_str());
			ImGui::Text(("SP: " + GetHexString16(processor.ReadStackPointer())).c_str());

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Interrupts");
			ImGui::Separator();

			ImGui::Text(("IF: " + GetHexString8(memoryMap.Read(GB_INTERRUPT_FLAG_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("IE: " + GetHexString8(memoryMap.Read(GB_INTERRUPT_ENABLE_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("IME: " + std::to_string(processor.GetInterruptMasterEnableFlag())).c_str());
		}

		EndWindow();
	}

	void EmulatorWindow::RenderSoundDebugWindow(APU& apu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		if (BeginWindow("Sound", &shouldRenderSoundDebugWindow, GENERAL_WINDOW_FLAGS))
		{
			ImGui::Text("Sound Control Registers");
			ImGui::Separator();
			// TODO: Revisit when NR50 and NR51 are implemented.
			ImGui::Text(("NR50: " + GetHexString16(0)).c_str());
			ImGui::Text(("NR51: " + GetHexString16(0)).c_str());
			ImGui::Text(("NR52: " + GetHexString16(apu.ReadNR52())).c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			bool isChannel1Connected = apu.IsChannel1Connected();
			ImGui::Checkbox("Channel 1", &isChannel1Connected);
			apu.SetChannel1ConnectionStatus(isChannel1Connected);
			ImGui::Separator();
			ImGui::Text(("NR10: " + GetHexString8(apu.ReadNR10())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR11: " + GetHexString8(apu.ReadNR11())).c_str());
			ImGui::Text(("NR12: " + GetHexString8(apu.ReadNR12())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR13: " + GetHexString8(apu.ReadNR13())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR14: " + GetHexString8(apu.ReadNR14())).c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			bool isChannel2Connected = apu.IsChannel2Connected();
			ImGui::Checkbox("Channel 2", &isChannel2Connected);
			apu.SetChannel2ConnectionStatus(isChannel2Connected);
			ImGui::Separator();
			ImGui::Text(("NR21: " + GetHexString8(apu.ReadNR21())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR22: " + GetHexString8(apu.ReadNR22())).c_str());
			ImGui::Text(("NR23: " + GetHexString8(apu.ReadNR23())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR24: " + GetHexString8(apu.ReadNR24())).c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			bool isChannel3Connected = apu.IsChannel3Connected();
			ImGui::Checkbox("Channel 3", &isChannel3Connected);
			apu.SetChannel3ConnectionStatus(isChannel3Connected);
			ImGui::Separator();
			ImGui::Text(("NR30: " + GetHexString8(apu.ReadNR30())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR31: " + GetHexString8(apu.ReadNR31())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR32: " + GetHexString8(apu.ReadNR32())).c_str());
			ImGui::Text(("NR33: " + GetHexString8(apu.ReadNR33())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR34: " + GetHexString8(apu.ReadNR34())).c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			bool isChannel4Connected = apu.IsChannel4Connected();
			ImGui::Checkbox("Channel 4", &isChannel4Connected);
			apu.SetChannel4ConnectionStatus(isChannel4Connected);
			ImGui::Separator();
			ImGui::Text(("NR41: " + GetHexString8(apu.ReadNR41())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR42: " + GetHexString8(apu.ReadNR42())).c_str());
			ImGui::Text(("NR43: " + GetHexString8(apu.ReadNR43())).c_str());
			ImGui::SameLine();
			ImGui::Text(("NR44: " + GetHexString8(apu.ReadNR44())).c_str());
		}

		EndWindow();
	}

	void EmulatorWindow::RenderJoypadDebugWindow(const Joypad& joypad)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		if (BeginWindow("Joypad", &shouldRenderJoypadDebugWindow, GENERAL_WINDOW_FLAGS))
		{
			ImGui::Text(("JOYP: " + GetHexString8(joypad.Read())).c_str());
			ImGui::Separator();
			ImGui::BeginDisabled();

			bool isDownOrStartPressed = joypad.IsDownOrStartPressed();
			bool isUpOrSelectPressed = joypad.IsUpOrSelectPressed();
			bool isLeftOrBPressed = joypad.IsLeftOrBPressed();
			bool isRightOrAPressed = joypad.IsRightOrAPressed();

			ImGui::Checkbox(" Down/Start Pressed", &isDownOrStartPressed);
			ImGui::Checkbox(" Up/Select Pressed", &isUpOrSelectPressed);
			ImGui::Checkbox(" Left/B Pressed", &isLeftOrBPressed);
			ImGui::Checkbox(" Right/A Pressed", &isRightOrAPressed);

			ImGui::EndDisabled();
		}

		EndWindow();
	}

	void EmulatorWindow::RenderVideoRegistersDebugWindow(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing information about the video-related registers.
		if (BeginWindow("Video", &shouldRenderVideoRegistersDebugWindow, GENERAL_WINDOW_FLAGS))
		{
			ImGui::Text("Registers");
			ImGui::Separator();

			ImGui::Text(("LCDC: " + GetHexString8(ppu.ReadLCDC())).c_str());
			ImGui::Text(("STAT: " + GetHexString8(ppu.ReadLCDSTAT())).c_str());
			ImGui::Text(("LY:   " + GetHexString8(ppu.ReadLY())).c_str());
			ImGui::Text(("LYC:  " + GetHexString8(ppu.ReadLYC())).c_str());
			ImGui::Text(("SCY:  " + GetHexString8(ppu.ReadSCY())).c_str());
			ImGui::Text(("SCX:  " + GetHexString8(ppu.ReadSCX())).c_str());
			ImGui::Text(("WY:   " + GetHexString8(ppu.ReadWY())).c_str());
			ImGui::Text(("WX:   " + GetHexString8(ppu.ReadWX())).c_str());

			ImGui::Separator();
		}

		EndWindow();
	}

	void EmulatorWindow::RenderTilesDebugWindow(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing all of the tiles currently in VRAM;
		RenderWindowWithFramebuffer("Tiles", ppu.GetTileDebugFramebuffer(), &shouldRenderTilesDebugWindow);
	}

	void EmulatorWindow::RenderSpritesDebugWindow(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing all of the sprites currently in VRAM.
		RenderWindowWithFramebuffer("Sprites", ppu.GetSpriteDebugFramebuffer(), &shouldRenderSpritesDebugWindow);
	}

	void EmulatorWindow::RenderBackgroundTileMapDebugWindow(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing the background tile map.
		RenderWindowWithFramebuffer("Background Tile Map", ppu.GetBackgroundMapDebugFramebuffer(), &shouldRenderBackgroundTileMapDebugWindow);
	}

	void EmulatorWindow::RenderWindowTileMapDebugWindow(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing the window tile map.
		RenderWindowWithFramebuffer("Window Tile Map", ppu.GetWindowMapDebugFramebuffer(), &shouldRenderWindowTileMapDebugWindow);
	}

	void EmulatorWindow::RenderTimerDebugWindow(const Timer& timer)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		if (BeginWindow("Timer", &shouldRenderTimerDebugWindow, GENERAL_WINDOW_FLAGS))
		{
			ImGui::Text("Registers");
			ImGui::Separator();

			ImGui::Text(("DIV:  " + GetHexString8(timer.GetDividerRegister())).c_str());
			ImGui::Text(("TIMA: " + GetHexString8(timer.GetTimerCounter())).c_str());
			ImGui::Text(("TMA:  " + GetHexString8(timer.GetTimerModulo())).c_str());
			ImGui::Text(("TAC:  " + GetHexString8(timer.GetTimerControlRegister())).c_str());

			ImGui::Separator();
		}

		EndWindow();
	}

	void EmulatorWindow::RenderLogWindow(const std::string& logEntries)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window containing log entries.
		if (BeginWindow("Logs", &shouldRenderLogWindow, GENERAL_WINDOW_FLAGS | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::Checkbox("Auto Scroll To Bottom", &shouldAutoScrollLogsToBottom);

			ImGui::Separator();

			ImVec2 size = ImGui::GetContentRegionAvail();
			size.y -= 25;

			if (ImGui::BeginChild("Log Entries", size))
			{
				// TextUnformatted() is used here since the other text rendering functions
				// seem to have a limit on the string's size.
				ImGui::TextUnformatted(logEntries.c_str());

				if (shouldAutoScrollLogsToBottom)
					ImGui::SetScrollHereY(0.999f);
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Clear"))
				clearButtonPressedCallback();
		}

		EndWindow();
	}

	void EmulatorWindow::RenderSettingsWindow(PPU& ppu, APU& apu, Joypad& joypad, InputManager& inputManager, Cartridge& cartridge)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(610, 541), ImGuiCond_FirstUseEver);

		if (BeginWindow("Settings", &shouldRenderSettingsWindow, GENERAL_WINDOW_FLAGS))
		{
			static int selectedWindow = SETTINGS_VIDEO_WINDOW_ID;

			if (ImGui::BeginChild("Settings Left", ImVec2(175, 0), true))
			{
				RenderSettingsWindowSelectableItem("Video", SETTINGS_VIDEO_WINDOW_ID, selectedWindow);
				RenderSettingsWindowSelectableItem("Audio", SETTINGS_AUDIO_WINDOW_ID, selectedWindow);
				RenderSettingsWindowSelectableItem("Controller/Keyboard", SETTINGS_CONTROLLER_AND_KEYBOARD_WINDOW_ID, selectedWindow);
				RenderSettingsWindowSelectableItem("Saved Data", SETTINGS_SAVED_DATA_WINDOW_ID, selectedWindow);
			}

			ImGui::Separator();
			ImGui::EndChild();

			ImGui::SameLine();
			if (ImGui::BeginChild("Settings Right"))
			{
				switch (selectedWindow)
				{
				case SETTINGS_VIDEO_WINDOW_ID:
					RenderVideoSettingsWindow(ppu);
					break;
				case SETTINGS_AUDIO_WINDOW_ID:
					RenderAudioSettingsWindow(apu);
					break;
				case SETTINGS_CONTROLLER_AND_KEYBOARD_WINDOW_ID:
					RenderControllerAndKeyboardSettingsWindow(joypad, inputManager);
					break;
				case SETTINGS_SAVED_DATA_WINDOW_ID:
					RenderSavedDataSettingsWindow(cartridge);
					break;
				}
			}

			ImGui::EndChild();
		}

		EndWindow();
	}

	void EmulatorWindow::RenderSettingsWindowSelectableItem(const std::string& label, int selectableID, int& currentSelectionID)
	{
		bool isSelected = currentSelectionID == selectableID;

		if (BeginSelectable(label.c_str(), isSelected))
			currentSelectionID = selectableID;

		EndSelectable(isSelected);
	}

	void EmulatorWindow::RenderVideoSettingsWindow(PPU& ppu)
	{
		static bool isPaletteTintEditorOpen = false;
		static uint8_t selectedPaletteTintID = BACKGROUND_AND_WINDOW_PALETTE_TINT_ID_0;

		static uint16_t paletteAddress = 0;
		static uint8_t colorIndex = 0;
		static std::string selectedTintLabel = "";

		ImGui::BeginGroup();

		if (ImGui::BeginChild("Video Settings"))
		{
			ImGui::Text("Video");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Palette Tints");
			ImGui::Separator();

			ImGui::Text("Background (BGP): ");
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 0", GB_BACKGROUND_PALETTE_ADDRESS, 0, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 1", GB_BACKGROUND_PALETTE_ADDRESS, 1, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 2", GB_BACKGROUND_PALETTE_ADDRESS, 2, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 3", GB_BACKGROUND_PALETTE_ADDRESS, 3, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);

			ImGui::Text("Sprite 0 (OBP0):  ");
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 0", GB_SPRITE_PALETTE_0_ADDRESS, 0, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 1", GB_SPRITE_PALETTE_0_ADDRESS, 1, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 2", GB_SPRITE_PALETTE_0_ADDRESS, 2, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 3", GB_SPRITE_PALETTE_0_ADDRESS, 3, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);

			ImGui::Text("Sprite 1 (OBP1):  ");
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP1 0", GB_SPRITE_PALETTE_1_ADDRESS, 0, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP1 1", GB_SPRITE_PALETTE_1_ADDRESS, 1, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP1 2", GB_SPRITE_PALETTE_1_ADDRESS, 2, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP1 3", GB_SPRITE_PALETTE_1_ADDRESS, 3, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
		}

		ImGui::EndChild();
		ImGui::EndGroup();

		if (isPaletteTintEditorOpen)
		{
			ImGui::SetNextWindowSize(ImVec2(250, 250));
			if (BeginWindow("Palette Editor", &isPaletteTintEditorOpen, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
			{
				Color rawTint = ppu.GetPaletteTint(paletteAddress, colorIndex);
				ImVec4 tint = ConvertColorToImVec4(rawTint);
				ImGui::ColorPicker3(selectedTintLabel.c_str(), &tint.x);
				ppu.SetPaletteTint(paletteAddress, colorIndex, ConvertImVec4ToColor(tint));
			}

			EndWindow();
		}
	}

	void EmulatorWindow::RenderColorPaletteButton(PPU& ppu, const std::string& label, uint16_t paletteAddress, uint8_t colorIndex, uint16_t& outPaletteAddress, uint8_t& outColorIndex, std::string& outLabel, bool& isColorPickerOpened)
	{
		Color rawTint = ppu.GetPaletteTint(paletteAddress, colorIndex);
		if (ImGui::ColorButton(label.c_str(), ConvertColorToImVec4(rawTint), ImGuiColorEditFlags_PickerHueWheel))
		{
			isColorPickerOpened = true;
			outPaletteAddress = paletteAddress;
			outColorIndex = colorIndex;
			outLabel = label;
		}
	}

	void EmulatorWindow::RenderAudioSettingsWindow(APU& apu)
	{
		ImGui::BeginGroup();

		if (ImGui::BeginChild("Audio Settings"))
		{
			ImGui::Text("Audio");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			const std::vector<std::string>& outputDeviceNames = apu.GetAllOutputDeviceNames();
			const std::string& selectedOutputDevice = apu.GetCurrentOutputDeviceName();

			// Render list of audio output devices.
			ImGui::Text("Output Device:");
			ImGui::SameLine();

			float minItemWidth = 50.0f;
			ImGui::SetNextItemWidth(std::max(ImGui::GetContentRegionAvail().x, minItemWidth));
			if (ImGui::BeginCombo("##Audio Output Device", selectedOutputDevice.c_str()))
			{
				for (const std::string& name : outputDeviceNames)
				{
					bool isSelected = selectedOutputDevice == name;

					// If the selectable is pressed, then set the selected device as the APU's output device.
					if (BeginSelectable(name.c_str(), isSelected, ImGuiSelectableFlags_None))
						apu.SetOutputDevice(name);

					EndSelectable(isSelected);
				}

				ImGui::EndCombo();
			}

			ImGui::Spacing();

			// Render volume slider.
			float volume = apu.GetMasterVolume() * MAX_VOLUME;
			ImGui::Text("Volume:       ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(std::max(ImGui::GetContentRegionAvail().x, minItemWidth));
			ImGui::SliderFloat("##Audio Settings Volume ", &volume, 0.0f, MAX_VOLUME, "%.0f", ImGuiSliderFlags_None);
			apu.SetMasterVolume(volume / MAX_VOLUME);

			ImGui::Spacing();

			// Render mute button.
			bool isMuted = apu.IsMuted();
			ImGui::Text("Mute:         ");
			ImGui::SameLine();
			ImGui::Checkbox("##Audio Settings Mute", &isMuted);
			apu.Mute(isMuted);
		}

		ImGui::EndChild();
		ImGui::EndGroup();
	}

	void EmulatorWindow::RenderControllerAndKeyboardSettingsWindow(Joypad& joypad, InputManager& inputManager)
	{
		ImGui::BeginGroup();

		if (ImGui::BeginChild("Controller/Keyboard Settings"))
		{
			ImGui::Text("Controller/Keyboard");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();


			const std::vector<std::string>& controllerNames = inputManager.GetAllControllerNames();

			if (controllerNames.size() > 0)
			{
				const std::string& selectedController = inputManager.GetCurrentControllerName();

				// Render list of controllers.
				ImGui::Text("Controller:");
				ImGui::SameLine();

				float minItemWidth = 50.0f;
				ImGui::SetNextItemWidth(std::max(ImGui::GetContentRegionAvail().x, minItemWidth));
				if (ImGui::BeginCombo("##Controllers", selectedController.c_str()))
				{
					for (const std::string& name : controllerNames)
					{
						bool isSelected = selectedController == name;

						// If the selectable is pressed, then set the selected controller as the input manager's controller.
						if (BeginSelectable(name.c_str(), isSelected, ImGuiSelectableFlags_None))
							inputManager.SetController(name);

						EndSelectable(isSelected);
					}

					ImGui::EndCombo();
				}
			}

			float initColumnWidth = 125;
			int columnCount = 3;

			auto& colors = ImGui::GetStyle().Colors;
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors[ImGuiCol_TabActive]);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, colors[ImGuiCol_TabActive]);

			ImGui::Spacing();
			ImGui::Text("Button Mapping");
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::BeginTable("Input Mapping", columnCount, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
			{
				ImGui::TableSetupColumn(" Game Boy", ImGuiTableColumnFlags_None, initColumnWidth);
				ImGui::TableSetupColumn("Controller", ImGuiTableColumnFlags_None, initColumnWidth);
				ImGui::TableSetupColumn("Keyboard", ImGuiTableColumnFlags_None, initColumnWidth);
				ImGui::TableHeadersRow();

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				for (int r = 0; r < GBBUTTON_STRINGS.size(); r++)
				{
					ImGui::TableNextRow();
					auto gbButton = static_cast<GBButton>(r);

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.12f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));

					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.075f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					for (int c = 0; c < columnCount; c++)
					{
						ImGui::TableNextColumn();

						float comboBoxWidth = std::max(ImGui::GetContentRegionAvail().x, 30.0f);
						switch (c)
						{
						case 0:
							ImGui::Text((" " + GBBUTTON_STRINGS.at(static_cast<GBButton>(r))).c_str());
							break;
						case 1:
							RenderControllerButtonComboBox(joypad, gbButton, r, comboBoxWidth);
							break;
						case 2:
							RenderKeyCodeComboBox(joypad, gbButton, r, comboBoxWidth);
							break;
						}
					}

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}

				ImGui::EndTable();
			}
		}

		ImGui::EndChild();
		ImGui::EndGroup();
	}

	void EmulatorWindow::RenderControllerButtonComboBox(Joypad& joypad, GBButton gbButton, int row, float width)
	{
		ControllerButtonCode selectedButton = joypad.GetControllerButtonCode(gbButton);
		ImGui::SetNextItemWidth(width);

		if (ImGui::BeginCombo(("##Controller Button" + std::to_string(row)).c_str(), CONTROLLER_BUTTON_STRINGS.at(selectedButton).c_str()))
		{
			for (const std::pair<ControllerButtonCode, std::string>& pair : CONTROLLER_BUTTON_STRINGS)
			{
				if (BeginSelectable(pair.second.c_str(), selectedButton == pair.first))
					joypad.SetControllerButtonCode(gbButton, pair.first);

				EndSelectable(selectedButton == pair.first);
			}

			ImGui::EndCombo();
		}
	}

	void EmulatorWindow::RenderKeyCodeComboBox(Joypad& joypad, GBButton gbButton, int row, float width)
	{
		KeyCode selectedKey = joypad.GetKeyCode(gbButton);
		ImGui::SetNextItemWidth(width);

		if (ImGui::BeginCombo(("##Key Code" + std::to_string(row)).c_str(), KEYCODE_STRINGS.at(selectedKey).c_str()))
		{
			for (const std::pair<KeyCode, std::string>& pair : KEYCODE_STRINGS)
			{
				if (BeginSelectable(pair.second.c_str(), selectedKey == pair.first))
					joypad.SetKeyCode(gbButton, pair.first);

				EndSelectable(selectedKey == pair.first);
			}

			ImGui::EndCombo();
		}
	}

	void EmulatorWindow::RenderSavedDataSettingsWindow(Cartridge& cartridge)
	{
		ImGui::BeginGroup();

		if (ImGui::BeginChild("Saved Data Settings"))
		{
			ImGui::Text("Saved Data");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			SavedDataSearchType currentSavedDataSearchType = cartridge.GetSavedDataSearchType();

			ImGui::Text("Saved Data Location: ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(std::max(ImGui::GetContentRegionAvail().x, 50.0f));
			if (ImGui::BeginCombo("##Saved Data Search Type", SAVED_DATA_SEARCH_TYPE_STRINGS.at(currentSavedDataSearchType).c_str()))
			{
				for (const std::pair<SavedDataSearchType, std::string>& pair : SAVED_DATA_SEARCH_TYPE_STRINGS)
				{
					bool isSelected = pair.first == currentSavedDataSearchType;
					if (BeginSelectable(pair.second.c_str(), isSelected))
					{
						currentSavedDataSearchType = pair.first;

						// Update the cartridge's saved data search type.
						cartridge.SetSavedDataSearchType(currentSavedDataSearchType);
					}

					EndSelectable(isSelected);
				}

				ImGui::EndCombo();
			}
		}

		ImGui::EndChild();
		ImGui::EndGroup();
	}

	void EmulatorWindow::ApplyTheme()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 225;
		style.WindowMenuButtonPosition = ImGuiDir_None;

		float roundness = 1.0f;
		style.ScrollbarRounding = roundness;
		style.FrameRounding = roundness;
		style.GrabRounding = roundness;
		style.TabRounding = roundness;

		auto& colors = style.Colors;

		// Window
		colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);

		// Menu bar
		colors[ImGuiCol_MenuBarBg] = colors[ImGuiCol_WindowBg];

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
		colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];

		// Text
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

		// Tabs
		colors[ImGuiCol_Tab] = colors[ImGuiCol_TitleBg];
		colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.52f, 0.7f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.42f, 0.6f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_TitleBg];
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);

		// Collapsing headers, tree nodes, selectables, and menu items
		colors[ImGuiCol_Header] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.8f, 1.00f);

		// Separators
		colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_TabHovered];

		// Background for checkboxes, radio buttons, sliders, and text input fields.
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

		// Checkbox 
		colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];

		// Buttons
		colors[ImGuiCol_Button] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_ButtonHovered] = colors[ImGuiCol_TabHovered];
		colors[ImGuiCol_ButtonActive] = ImVec4(0.48f, 0.72f, 0.9f, 1.00f);

		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_ResizeGripActive] = colors[ImGuiCol_TabHovered];

		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.175f, 0.175f, 0.175f, 1.00f);

		colors[ImGuiCol_Border] = colors[ImGuiCol_TabUnfocusedActive];
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_SliderGrabActive] = colors[ImGuiCol_TabHovered];

		colors[ImGuiCol_PopupBg] = colors[ImGuiCol_WindowBg];

		// Tables
		colors[ImGuiCol_TableHeaderBg] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_TableBorderStrong] = colors[ImGuiCol_TabUnfocusedActive];
		colors[ImGuiCol_TableBorderLight] = colors[ImGuiCol_TabUnfocusedActive];
		colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.27f, 0.27f, 0.27f, 0.1f);

		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		colors[ImGuiCol_ScrollbarGrab] = colors[ImGuiCol_TabUnfocusedActive];
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	}

	std::string EmulatorWindow::GetPathFromFileBrowser(const std::string& filterFriendlyName, const std::string& filters)
	{
		std::string resultPath = "";
		nfdchar_t* outPath = nullptr;
		nfdfilteritem_t filterItem[1] = { {filterFriendlyName.c_str(), filters.c_str()} };
		nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);

		switch (result)
		{
		case NFD_OKAY:
			resultPath = std::string(outPath);
			free(outPath);
			break;
		case NFD_CANCEL:
			break;
		default:
			Logger::WriteError("Failed to open file browser. Error: " + std::string(NFD_GetError()));
			break;
		}

		return resultPath;
	}

	bool EmulatorWindow::BeginSelectable(const std::string& label, bool isSelected, ImGuiSelectableFlags flags, const ImVec2& size)
	{
		if (isSelected)
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyle().Colors[ImGuiCol_TabActive]);

		return ImGui::Selectable(label.c_str(), isSelected, flags, size);
	}

	void EmulatorWindow::EndSelectable(bool isSelected)
	{
		if (isSelected)
			ImGui::PopStyleColor();
	}

	void EmulatorWindow::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(sdlRenderer);
	}

	void EmulatorWindow::SetPauseButtonLabel(const std::string& label)
	{
		pauseButtonLabel = label;
	}

	ImVec4 EmulatorWindow::ConvertColorToImVec4(Color& color) const
	{
		float r = color.r / 255.0f;
		float g = color.g / 255.0f;
		float b = color.b / 255.0f;
		float a = color.a / 255.0f;

		return ImVec4(r, g, b, a);
	}

	Color EmulatorWindow::ConvertImVec4ToColor(ImVec4& vec) const
	{
		uint8_t r = static_cast<uint8_t>(vec.x * 255);
		uint8_t g = static_cast<uint8_t>(vec.y * 255);
		uint8_t b = static_cast<uint8_t>(vec.z * 255);
		uint8_t a = static_cast<uint8_t>(vec.w * 255);

		return { r, g, b, a };
	}
}