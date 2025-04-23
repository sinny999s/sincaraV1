#include "menu/menu.h"

#include <gl/GL.h>
#include <mutex>
#include <Shlobj.h>

#include "minhook/minhook.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"
#include "util/logger/logger.h"
#include "util/trimmer.h"
#include "configManager/settings.h"

#include "fonts/inter.h"

#include "base.h"
#include <imgui/imgui_internal.h>

std::once_flag setupFlag;
std::atomic_flag clipCursor = ATOMIC_FLAG_INIT;
RECT originalClip;

typedef bool(__stdcall* template_wglSwapBuffers) (HDC hdc);
template_wglSwapBuffers original_wglSwapBuffers;
bool __stdcall hook_wglSwapBuffers(_In_ HDC hdc)
{
	// handling fullscreen context switching before we set the new hwnd, so we can compare them
	
	// if the cached hwnd isnt equal to the current one
	// info: window handles change when you enter/exit fullscreen
	if (Menu::HandleWindow != WindowFromDC(hdc) && Menu::Initialized)
	{
		// set handlewindow so that the wndproc can attach to new one before passing
		Menu::HandleWindow = WindowFromDC(hdc);

		// opengl context for font issue
		Menu::OriginalGLContext = wglGetCurrentContext();
		wglMakeCurrent(hdc, wglCreateContext(hdc));

		// uninitialize imgui opengl and win32 implementation
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplWin32_Shutdown();
		
		// reinitialize it
		ImGui_ImplWin32_Init(Menu::HandleWindow);
		ImGui_ImplOpenGL2_Init();

		// set wndproc
		Menu::Hook_wndProc();

		// revert to old ctx
		wglMakeCurrent(hdc, Menu::OriginalGLContext);

		// end detour
		return original_wglSwapBuffers(hdc);
	}

	Menu::HandleDeviceContext = hdc;
	Menu::HandleWindow = WindowFromDC(hdc);
	Menu::OriginalGLContext = wglGetCurrentContext();

	std::call_once(setupFlag, [&] {
		Menu::Hook_wndProc();
		Menu::SetupImgui();
	});

	wglMakeCurrent(Menu::HandleDeviceContext, Menu::MenuGLContext);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushFont(Menu::Font);

	if (Menu::Open)
	{
		if (clipCursor.test_and_set()) GetClipCursor(&originalClip);

		ClipCursor(NULL);
		Menu::RenderMenu();
	}
	else
	{
		// checking if originalClip is valid
		if (originalClip.right > originalClip.left && originalClip.bottom > originalClip.top)
		{
			ClipCursor(&originalClip);
			clipCursor.clear();
		}
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Overlay", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoBackground);

	Base::RenderLoop();

	ImGui::PopFont();
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	wglMakeCurrent(Menu::HandleDeviceContext, Menu::OriginalGLContext);
	return original_wglSwapBuffers(hdc);
}

void Menu::Hook_wglSwapBuffers()
{
	LPVOID wglSwapBuffers = (LPVOID)GetProcAddress(GetModuleHandle("opengl32.dll"), "wglSwapBuffers");
	MH_CreateHook(wglSwapBuffers, (LPVOID)hook_wglSwapBuffers, (LPVOID*)&original_wglSwapBuffers);
	MH_EnableHook(wglSwapBuffers);
}

void Menu::Unhook_wglSwapBuffers()
{	
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
}

void Menu::SetupImgui()
{
	Menu::MenuGLContext = wglCreateContext(Menu::HandleDeviceContext);
	wglMakeCurrent(Menu::HandleDeviceContext, Menu::MenuGLContext);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glOrtho(0, m_viewport[2], m_viewport[3], 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);

	Menu::CurrentImGuiContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	Menu::Font = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 16);
	Menu::FontBold = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 24);

	Menu::Font28 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 28);
	Menu::Font26 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 26);
	Menu::Font24 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 24);
	Menu::Font22 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 22);
	Menu::Font20 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 20);
	Menu::Font18 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 18);
	Menu::Font16 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 16);
	Menu::Font14 = io.Fonts->AddFontFromMemoryTTF(inter_regular, sizeof(inter_regular), 14);

	Menu::BoldFont28 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 28);
	Menu::BoldFont26 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 26);
	Menu::BoldFont24 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 24);
	Menu::BoldFont22 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 22);
	Menu::BoldFont20 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 20);
	Menu::BoldFont18 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 18);
	Menu::BoldFont16 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 16);
	Menu::BoldFont14 = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 14);

	SetupStyle();

	ImGui_ImplWin32_Init(Menu::HandleWindow);
	ImGui_ImplOpenGL2_Init();

	Menu::Initialized = true;
}
