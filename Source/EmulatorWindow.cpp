#include "nfd.h"
#include "EmulatorWindow.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/GBMemoryMapAddresses.hpp"

namespace SHG
{
	const char* DEFAULT_WINDOW_TITLE = "Game Boy Emulator";
	const uint16_t DEFAULT_WINDOW_WIDTH = 1024;
	const uint16_t DEFAULT_WINDOW_HEIGHT = 576;

	const ImGuiWindowFlags MAIN_WINDOW_FLAGS = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoSavedSettings;

	bool EmulatorWindow::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			// TODO: Print SDL error
			Logger::WriteError("SDL failed to initialize!");
			return false;
		}

		// TODO: Check for errors
		sdlWindow = SDL_CreateWindow(DEFAULT_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		sdlRenderer = SDL_CreateRenderer(sdlWindow, 0, 0);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow);
		ImGui_ImplSDLRenderer_Init(sdlRenderer);

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		return true;
	}

	void EmulatorWindow::Render(MemoryMap& memoryMap, PPU& ppu, CPU& processor, uint32_t cyclesPerSecond, std::string& logs)
	{
		StartFrame();
		ClearScreen();

		RenderMainWindow();
		RenderGameView(ppu);

		if (shouldRenderCPUWindow)
			RenderCPUWindow(processor, cyclesPerSecond);

		if (shouldRenderIOWindow)
			RenderIOWindow(memoryMap, processor);

		if (shouldRenderLogWindow)
			RenderLogWindow(logs);

		if (shouldRenderTilesWindow)
			RenderTilesWindow(ppu);

		if (shouldRenderSpritesWindow)
			RenderSpritesWindow(ppu);

		if (shouldRenderBackgroundTileMapWindow)
			RenderBackgroundTileMapWindow(ppu);

		if (shouldRenderWindowTileMapWindow)
			RenderWindowTileMapWindow(ppu);

		if (shouldRenderVideoRegistersWindow)
			RenderVideoRegistersWindow(ppu);

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

	void EmulatorWindow::RegisterFileSelectionCallback(FileSelectionEvent callback)
	{
		romFileSelectionCallback = callback;
	}

	void EmulatorWindow::RegisterPauseButtonCallback(SimpleEvent callback)
	{
		pauseButtonPressedCallback = callback;
	}

	void EmulatorWindow::RegisterStepButtonCallback(SimpleEvent callback)
	{
		stepButtonPressedCallback = callback;
	}

	void EmulatorWindow::RegisterClearButtonCallback(SimpleEvent callback)
	{
		clearButtonPressedCallback = callback;
	}

	void EmulatorWindow::RegisterQuitButtonCallback(SimpleEvent callback)
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
						NFD_OpenDialog(nullptr, nullptr, &outPath);

						if (outPath != nullptr)
						{
							romFileSelectionCallback(outPath);
							free(outPath);
						}
					}

					ImGui::EndMenu();
				}


				// Menu that enables the user to open debugging tools/windows.
				if (ImGui::BeginMenu("Tools"))
				{
					if (ImGui::MenuItem("CPU Debugger"))
						shouldRenderCPUWindow = true;

					if (ImGui::MenuItem("Log Window"))
						shouldRenderLogWindow = true;

					if (ImGui::MenuItem("IO Debugger"))
						shouldRenderIOWindow = true;

					if (ImGui::MenuItem("Tile Debugger"))
						shouldRenderTilesWindow = true;

					if (ImGui::MenuItem("Sprite Debugger"))
						shouldRenderSpritesWindow = true;

					if (ImGui::MenuItem("Background Tile Map"))
						shouldRenderBackgroundTileMapWindow = true;

					if (ImGui::MenuItem("Window Tile Map"))
						shouldRenderWindowTileMapWindow = true;

					if (ImGui::MenuItem("Video Registers"))
						shouldRenderVideoRegistersWindow = true;

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
	void EmulatorWindow::RenderGameView(PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window containing the currently loaded game, if any.
		RenderWindowWithFramebuffer("Game", ppu.GetFramebuffer());
	}

	void EmulatorWindow::RenderWindowWithFramebuffer(const std::string& title, Framebuffer& framebuffer, bool* isOpen)
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
				(contentRegionSize.x - imageSize.x) * 0.5,
				(contentRegionSize.y - imageSize.y) * 0.5 + 25));

			ImGui::Image((void*)framebuffer.GetTexture(), imageSize);
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderCPUWindow(CPU& processor, uint32_t cyclesPerSecond)
	{
		// By default, force the window to be docked.
		//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing CPU related information (clock speed, registers, etc.).
		if (ImGui::Begin("CPU", &shouldRenderCPUWindow))
		{
			ImGui::Text("Performance");
			ImGui::Separator();
			ImGui::Text(("Clock speed: " + std::to_string(cyclesPerSecond / 1000000.0) + " MHz").c_str());

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Registers");
			ImGui::Separator();

			// Print register values 
			ImGui::Text(("A: " + GetHexString8(processor.GetRegisterA().GetData())).c_str());
			ImGui::SameLine();
			ImGui::Text(("F: " + GetHexString8(processor.GetRegisterF().GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("B: " + GetHexString8(processor.GetRegisterB().GetData())).c_str());
			ImGui::SameLine();
			ImGui::Text(("C: " + GetHexString8(processor.GetRegisterC().GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("D: " + GetHexString8(processor.GetRegisterD().GetData())).c_str());
			ImGui::SameLine();
			ImGui::Text(("E: " + GetHexString8(processor.GetRegisterE().GetData())).c_str());

			ImGui::Text(("H: " + GetHexString8(processor.GetRegisterH().GetData())).c_str());
			ImGui::SameLine();
			ImGui::Text(("L: " + GetHexString8(processor.GetRegisterL().GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("PC: " + GetHexString16(processor.GetProgramCounter().GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("SP: " + GetHexString16(processor.GetStackPointer().GetData())).c_str());

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Last Instruction");
			ImGui::Separator();
			if (processor.IsPreviousInstructionValid())
				ImGui::Text(processor.GetCurrentInstruction().mnemonic);

			ImGui::Separator();
			if (ImGui::Button(pauseButtonLabel.c_str()))
				pauseButtonPressedCallback();

			ImGui::SameLine();
			ImGui::Button("Step");
				
			// If the step button is held down, then keep invoking the callback.
			if (ImGui::IsItemActive())
				stepButtonPressedCallback();
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderIOWindow(MemoryMap& memoryMap, CPU& processor)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing information about IO registers.
		if (ImGui::Begin("IO", &shouldRenderIOWindow))
		{
			ImGui::Text("Interrupts");
			ImGui::Text(("IF: " + GetHexString8(memoryMap.Read(GB_INTERRUPT_FLAG_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("IE: " + GetHexString8(memoryMap.Read(GB_INTERRUPT_ENABLE_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("IME: " + std::to_string(processor.GetInterruptMasterEnableFlag())).c_str());

			ImGui::Separator();
			ImGui::Text("Timer");
			ImGui::Text(("DIV: " + GetHexString8(memoryMap.Read(GB_DIV_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("TIMA: " + GetHexString8(memoryMap.Read(GB_TIMA_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("TMA: " + GetHexString8(memoryMap.Read(GB_TMA_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("TAC: " + GetHexString8(memoryMap.Read(GB_TAC_ADDRESS))).c_str());

			ImGui::Separator();
			ImGui::Text("Serial");
			ImGui::Text(("SB: " + GetHexString8(memoryMap.Read(GB_SB_ADDRESS))).c_str());
			ImGui::SameLine();
			ImGui::Text(("SC: " + GetHexString8(memoryMap.Read(GB_SC_ADDRESS))).c_str());

			// TODO: Print sound registers
			ImGui::Separator();
			ImGui::Text("Misc");
			ImGui::Text(("JOYP: " + GetHexString8(memoryMap.Read(GB_JOYP_ADDRESS))).c_str());
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderTilesWindow(PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing all of the tiles currently in VRAM;
		RenderWindowWithFramebuffer("Tiles", ppu.GetTileDebugFramebuffer(), &shouldRenderTilesWindow);
	}

	void EmulatorWindow::RenderSpritesWindow(PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing all of the sprites currently in VRAM.
		RenderWindowWithFramebuffer("Sprites", ppu.GetSpriteDebugFramebuffer(), &shouldRenderSpritesWindow);
	}

	void EmulatorWindow::RenderBackgroundTileMapWindow(PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing the background tile map.
		RenderWindowWithFramebuffer("Background Tile Map", ppu.GetBackgroundMapDebugFramebuffer(), &shouldRenderBackgroundTileMapWindow);
	}

	void EmulatorWindow::RenderWindowTileMapWindow(PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing the window tile map.
		RenderWindowWithFramebuffer("Window Tile Map", ppu.GetWindowMapDebugFramebuffer(), &shouldRenderWindowTileMapWindow);
	}

	void EmulatorWindow::RenderVideoRegistersWindow(PPU& ppu)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window showing information about the video-related registers.
		if (ImGui::Begin("Video", &shouldRenderVideoRegistersWindow))
		{
			ImGui::Text("Registers");
			ImGui::Separator();
			ImGui::Text(("LCDC: " + GetHexString8(ppu.GetLCDC()->GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("STAT: " + GetHexString8(ppu.GetLCDStatus()->GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("LY: " + GetHexString8(ppu.GetLY()->GetData())).c_str());

			ImGui::Text(("LYC: " + GetHexString8(ppu.GetLYC()->GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("SCY: " + GetHexString8(ppu.GetSCY()->GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("SCX: " + GetHexString8(ppu.GetSCX()->GetData())).c_str());

			ImGui::Text(("WY: " + GetHexString8(ppu.GetWY()->GetData())).c_str());
			//ImGui::SameLine();
			ImGui::Text(("WX: " + GetHexString8(ppu.GetWX()->GetData())).c_str());
			ImGui::Separator();
		}

		ImGui::End();
	}

	void EmulatorWindow::RenderLogWindow(std::string& logs)
	{
		// By default, force the window to be docked.
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

		// Window containing log entries.
		if (ImGui::Begin("Logs", &shouldRenderLogWindow, ImGuiWindowFlags_NoScrollbar))
		{
			if (ImGui::RadioButton("Trace", isTraceEnabled))
				isTraceEnabled = !isTraceEnabled;

			ImGui::Separator();

			ImVec2 size = ImGui::GetContentRegionAvail();
			size.y -= 25;

			if (ImGui::BeginChild("Log Entries", size))
			{
				// TextUnformatted() is used here since the other text rendering functions
				// seem to have a limit on the string's size.
				ImGui::TextUnformatted(logs.c_str());

				if (isTraceEnabled)
					ImGui::SetScrollHereY(0.999f);
			}
			ImGui::EndChild();
		
			ImGui::Separator();
			if (ImGui::Button("Clear"))
				clearButtonPressedCallback();
		}

		ImGui::End();
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
}