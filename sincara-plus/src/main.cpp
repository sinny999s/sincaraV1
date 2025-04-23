#include "main.h"
#include "imgui/imgui.h" // <-- Add this line
#include "base/menu/menu.h" // <-- And this line

#include <fstream>
#include <exception>

static LONG WINAPI GlobalExceptionHandler(EXCEPTION_POINTERS* pException)
{
	Base::WriteCrashReport(pException);
	return EXCEPTION_EXECUTE_HANDLER;
}

void Main::Init()
{
	Base::Init();

	const double targetFrameTime = 1.0 / 144.0; // 144 FPS cap

	while (Base::Running) {
		double frameStart = ImGui::GetTime();

		Menu::RenderMenu();

		double frameEnd = ImGui::GetTime();
		double elapsed = frameEnd - frameStart;
		if (elapsed < targetFrameTime) {
			DWORD sleepMs = static_cast<DWORD>((targetFrameTime - elapsed) * 1000.0);
			if (sleepMs > 0)
				Sleep(sleepMs);
		}
	}
}

void Main::Kill()
{
	Base::Kill();
	FreeLibraryAndExitThread(Main::HModule, 0);
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Main::HModule = hModule;
		DisableThreadLibraryCalls(hModule);
		//SetUnhandledExceptionFilter(GlobalExceptionHandler);
		HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Main::Init), hModule, 0, nullptr);

		if (hThread) CloseHandle(hThread);
	}

	return TRUE;
}