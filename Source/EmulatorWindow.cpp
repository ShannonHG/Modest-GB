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

	const std::array<std::string, 8> GB_BUTTONS =
	{
		"A", "B", "RIGHT", "LEFT", "UP", "DOWN", "START", "SELECT"
	};

	const std::array<std::string, 12> CONTROLLER_BUTTONS =
	{
		"A", "B", "X", "Y", "DPAD RIGHT", "DPAD LEFT",
		"DPAD UP", "DPAD DOWN", "RIGHT SHOULDER", "LEFT SHOULDER",
		"START", "MENU"
	};

	const std::array<std::string, 40> KEYS =
	{
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
		"B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
		"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"RIGHT ARROW", "LEFT ARROW", "UP ARROW", "DOWN ARROW"
	};

	const ImGuiWindowFlags MAIN_WINDOW_FLAGS = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoSavedSettings;

	bool EmulatorWindow::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		{
			Logger::WriteError("SDL failed to initialize. Error: " + std::string(SDL_GetError()));
			return false;
		}

		sdlWindow = SDL_CreateWindow(DEFAULT_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_SDL_WINDOW_WIDTH, DEFAULT_SDL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize = ImVec2(225, 150);

		return true;
	}

	int EmulatorWindow::GetWidth()
	{
		int width = 0;
		SDL_GetWindowSize(sdlWindow, &width, nullptr);
		return width;
	}

	int EmulatorWindow::GetHeight()
	{
		int height = 0;
		SDL_GetWindowSize(sdlWindow, nullptr, &height);
		return height;
	}

	void EmulatorWindow::SetSize(int width, int height)
	{
		SDL_SetWindowSize(sdlWindow, width, height);
	}

	int EmulatorWindow::GetX()
	{
		int x = 0;
		SDL_GetWindowPosition(sdlWindow, &x, nullptr);
		return x;
	}

	int EmulatorWindow::GetY()
	{
		int y = 0;
		SDL_GetWindowPosition(sdlWindow, &y, nullptr);
		return y;
	}

	void EmulatorWindow::SetPosition(int x, int y)
	{
		SDL_SetWindowPosition(sdlWindow, x, y);
	}

	void EmulatorWindow::Render(const MemoryMap& memoryMap, PPU& ppu, const CPU& processor, APU& apu, Joypad& joypad, const Timer& timer, uint32_t cyclesPerSecond, std::string& logEntries)
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
			RenderSettingsWindow(ppu, apu, joypad);

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

	bool EmulatorWindow::IsTraceEnabled()
	{
		return isTraceEnabled;
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
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Load ROM"))
					{
						// TODO: Opening the file dialog seems to cause a memory leak.
						nfdchar_t* outPath = nullptr;
						NFD_OpenDialog("gb,rom", nullptr, &outPath);

						if (outPath != nullptr)
						{
							romFileSelectionCallback(outPath);
							free(outPath);
						}
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

				ImGui::EndMenuBar();
			}
		}

		dockspaceID = ImGui::GetID("Dockspace");
		ImGui::DockSpace(dockspaceID, ImVec2(0, 0), ImGuiDockNodeFlags_None);
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
		if (ImGui::Begin(title.c_str(), isOpen))
		{
			ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();

			ImVec2 imageSize;

			// TODO: Fix flickering when the width and height are equal.
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


			// Position the game view in the center of the window.
			ImGui::SetCursorPos(ImVec2(
				(contentRegionSize.x - imageSize.x) * 0.5f,
				(contentRegionSize.y - imageSize.y) * 0.5f + 25));

			ImGui::Image((void*)framebuffer.GetTexture(), imageSize);
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderCPUDebugWindow(const CPU& processor, const MemoryMap& memoryMap, uint32_t cyclesPerSecond)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing CPU related information (clock speed, registers, etc.).
		if (ImGui::Begin("CPU", &shouldRenderCPUDebugWindow))
		{
			ImGui::Text("Performance");
			ImGui::Separator();
			ImGui::Text(("Clock speed: " + std::to_string(cyclesPerSecond / 1000000.0) + " MHz").c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			if (ImGui::Button(pauseButtonLabel.c_str()))
				pauseButtonPressedCallback();

			ImGui::SameLine();

			if (ImGui::Button("Step"))
				stepButtonPressedCallback();

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

		ImGui::End();
	}

	void EmulatorWindow::RenderSoundDebugWindow(APU& apu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Sound", &shouldRenderSoundDebugWindow))
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

		ImGui::End();
	}

	void EmulatorWindow::RenderJoypadDebugWindow(const Joypad& joypad)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Joypad", &shouldRenderJoypadDebugWindow))
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

		ImGui::End();
	}

	void EmulatorWindow::RenderVideoRegistersDebugWindow(const PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing information about the video-related registers.
		if (ImGui::Begin("Video", &shouldRenderVideoRegistersDebugWindow))
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

		ImGui::End();
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

		if (ImGui::Begin("Timer", &shouldRenderTimerDebugWindow))
		{
			ImGui::Text("Registers");
			ImGui::Separator();

			ImGui::Text(("DIV:  " + GetHexString8(timer.GetDividerRegister())).c_str());
			ImGui::Text(("TIMA: " + GetHexString8(timer.GetTimerCounter())).c_str());
			ImGui::Text(("TMA:  " + GetHexString8(timer.GetTimerModulo())).c_str());
			ImGui::Text(("TAC:  " + GetHexString8(timer.GetTimerControlRegister())).c_str());

			ImGui::Separator();
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderLogWindow(const std::string& logEntries)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window containing log entries.
		if (ImGui::Begin("Logs", &shouldRenderLogWindow, ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::Checkbox("Trace", &isTraceEnabled);

			static bool isLogWindowScrollLockedToBottom = false;

			ImGui::SameLine();
			ImGui::Checkbox("Auto Scroll To Bottom", &isLogWindowScrollLockedToBottom);

			ImGui::Separator();

			ImVec2 size = ImGui::GetContentRegionAvail();
			size.y -= 25;

			if (ImGui::BeginChild("Log Entries", size))
			{
				// TextUnformatted() is used here since the other text rendering functions
				// seem to have a limit on the string's size.
				ImGui::TextUnformatted(logEntries.c_str());

				if (isLogWindowScrollLockedToBottom)
					ImGui::SetScrollHereY(0.999f);
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Clear"))
				clearButtonPressedCallback();
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderSettingsWindow(PPU& ppu, APU& apu, Joypad& joypad)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(610, 541), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Settings", &shouldRenderSettingsWindow))
		{
			static int selectedWindow = SETTINGS_VIDEO_WINDOW_ID;

			if (ImGui::BeginChild("Left", ImVec2(175, 0), true))
			{
				if (ImGui::Selectable("Video", selectedWindow == 0))
					selectedWindow = SETTINGS_VIDEO_WINDOW_ID;

				ImGui::Spacing();
				if (ImGui::Selectable("Audio", selectedWindow == 1))
					selectedWindow = SETTINGS_AUDIO_WINDOW_ID;

				ImGui::Spacing();
				if (ImGui::Selectable("Controller/Keyboard", selectedWindow == 2))
					selectedWindow = SETTINGS_CONTROLLER_AND_KEYBOARD_WINDOW_ID;
			}

			ImGui::Separator();
			ImGui::EndChild();

			ImGui::SameLine();
			if (ImGui::BeginChild("Right"))
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
					RenderControllerAndKeyboardSettingsWindow(joypad);
					break;
				}
			}

			ImGui::EndChild();
		}

		ImGui::End();
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

			ImGui::Text("Background And Window (BGP):");
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 0", GB_BACKGROUND_PALETTE_ADDRESS, 0, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 1", GB_BACKGROUND_PALETTE_ADDRESS, 1, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 2", GB_BACKGROUND_PALETTE_ADDRESS, 2, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "BGP 3", GB_BACKGROUND_PALETTE_ADDRESS, 3, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);

			ImGui::Text("Sprite 0 (OBP0):            ");
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 0", GB_SPRITE_PALETTE_0_ADDRESS, 0, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 1", GB_SPRITE_PALETTE_0_ADDRESS, 1, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 2", GB_SPRITE_PALETTE_0_ADDRESS, 2, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);
			ImGui::SameLine();
			RenderColorPaletteButton(ppu, "OBP0 3", GB_SPRITE_PALETTE_0_ADDRESS, 3, paletteAddress, colorIndex, selectedTintLabel, isPaletteTintEditorOpen);

			ImGui::Text("Sprite 1 (OBP1):            ");
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
			if (ImGui::Begin("Palette Editor", &isPaletteTintEditorOpen, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
			{
				Color rawTint = ppu.GetPaletteTint(paletteAddress, colorIndex);
				ImVec4 tint = ConvertColorToImVec4(rawTint);
				ImGui::ColorPicker3(selectedTintLabel.c_str(), &tint.x);
				ppu.SetPaletteTint(paletteAddress, colorIndex, ConvertImVec4ToColor(tint));
			}

			ImGui::End();
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
			std::string selectedOutputDevice = apu.GetCurrentOutputDeviceName();

			// Render list of audio output devices.
			ImGui::Text("Output Device:");
			ImGui::SameLine();

			if (ImGui::BeginCombo("##Audio Output Device", selectedOutputDevice.c_str()))
			{
				for (const std::string& name : outputDeviceNames)
				{
					// If the selectable is pressed, then set the selected device as the APU's output device.
					if (ImGui::Selectable(name.c_str(), selectedOutputDevice == name))
						apu.SetOutputDevice(name);
				}

				ImGui::EndCombo();
			}

			ImGui::Spacing();

			// Render volume slider.
			float volume = apu.GetMasterVolume() * MAX_VOLUME;
			ImGui::Text("Volume:       ");
			ImGui::SameLine();
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

	void EmulatorWindow::RenderControllerAndKeyboardSettingsWindow(Joypad& joypad)
	{
		ImGui::BeginGroup();

		if (ImGui::BeginChild("Controller/Keyboard Settings"))
		{
			ImGui::Text("Controller/Keyboard Mapping");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			float columnWidth = 125;
			int columnCount = 3;

			if (ImGui::BeginTable("Test", columnCount, ImGuiTableFlags_Borders, ImVec2(columnWidth * columnCount + 30, 0)))
			{
				ImGui::TableSetupColumn("Game Boy", ImGuiTableColumnFlags_WidthFixed, columnWidth);
				ImGui::TableSetupColumn("Controller", ImGuiTableColumnFlags_WidthFixed, columnWidth);
				ImGui::TableSetupColumn("Keyboard", ImGuiTableColumnFlags_WidthFixed, columnWidth);
				ImGui::TableHeadersRow();

				for (int r = 0; r < GB_BUTTONS.size(); r++)
				{
					ImGui::TableNextRow();
					auto gbButton = static_cast<GBButton>(r);

					for (int c = 0; c < columnCount; c++)
					{
						ImGui::TableNextColumn();

						switch (c)
						{
						case 0:
							ImGui::Text(GB_BUTTONS.at(r).c_str());
							break;
						case 1:
							RenderControllerButtonComboBox(joypad, gbButton, r, columnWidth);
							break;
						case 2:
							RenderKeyCodeComboBox(joypad, gbButton, r, columnWidth);
							break;
						}
					}
				}

				ImGui::EndTable();
			}
		}

		ImGui::EndChild();
		ImGui::EndGroup();
	}

	void EmulatorWindow::RenderControllerButtonComboBox(Joypad& joypad, GBButton gbButton, int row, float width)
	{
		int selectedButtonIndex = static_cast<int>(joypad.GetControllerButtonCode(gbButton));
		ImGui::SetNextItemWidth(width);

		if (ImGui::BeginCombo("##Controller Button", CONTROLLER_BUTTONS[selectedButtonIndex].c_str()))
		{
			for (int i = 0; i < CONTROLLER_BUTTONS.size(); i++)
			{
				if (ImGui::Selectable(CONTROLLER_BUTTONS[i].c_str(), selectedButtonIndex == i))
					joypad.SetControllerButtonCode(gbButton, static_cast<ControllerButtonCode>(i));
			}

			ImGui::EndCombo();
		}
	}

	void EmulatorWindow::RenderKeyCodeComboBox(Joypad& joypad, GBButton gbButton, int row, float width)
	{
		int selectedKeyIndex = static_cast<int>(joypad.GetKeyCode(gbButton));
		ImGui::SetNextItemWidth(width);

		if (ImGui::BeginCombo("##Key Code ", KEYS[selectedKeyIndex].c_str()))
		{
			for (int i = 0; i < KEYS.size(); i++)
			{
				if (ImGui::Selectable(KEYS[i].c_str(), selectedKeyIndex == i))
					joypad.SetKeyCode(gbButton, static_cast<KeyCode>(i));
			}

			ImGui::EndCombo();
		}
	}

	void EmulatorWindow::RenderSavedDataSettingsWindow()
	{
	
	}

	void EmulatorWindow::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(sdlRenderer);
	}

	SDL_Window* EmulatorWindow::GetSDLWindow()
	{
		return sdlWindow;
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