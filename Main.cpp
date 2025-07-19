#pragma once

#include "Functions.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance,		// **** Main Windows function **** //
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nShowCmd)
{
	AllocConsole();	// initialize console window

	FILE* fptr1;
	FILE* fptr2;
	FILE* fptr3;

	freopen_s(&fptr1, "conin$", "r", stdin);
	freopen_s(&fptr2, "conout$", "w", stdout);
	freopen_s(&fptr3, "conout$", "w", stderr);

	if (fptr1 == nullptr) MessageBox(0, L"Console Initialization Failed: stdin", L"Error", MB_OK);
	if (fptr2 == nullptr) MessageBox(0, L"Console Initialization Failed: stdout", L"Error", MB_OK);
	if (fptr3 == nullptr) MessageBox(0, L"Console Initialization Failed: stderr", L"Error", MB_OK);

	Parse_data();

	if (not InitializeWindow(hInstance, nShowCmd, screen_resolution.x, screen_resolution.y, windowed))
	{
		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (not InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (not InitializeDirect3d11App())
	{
		MessageBox(0, L"Direct3D Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	Initialize_game();

	print(runtime(NEWL + NEWL + "*** Entering main loop.. ***" + NEWL));
	while (running)	// **** Main Loop **** //
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))	// process window messages
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);	// @ WndProc()
		}

		if (paused == false)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();

			Refresh_Input();

			ImGui::NewFrame();

			Process_Input();

			Step();

			Render();
		}
	}
	print(runtime(NEWL + "*** Main loop terminated. ***" + NEWL));

	CleanUp();
	std::cin.ignore();	// keep console from closing
	return msg.wParam;
}