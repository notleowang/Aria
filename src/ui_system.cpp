#include "ui_system.hpp"

void UISystem::init() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UISystem::showWindows() {
	static bool show_main_menu_window = true;

	if (show_main_menu_window) showMainMenu(&show_main_menu_window);
}

void UISystem::showMainMenu(bool* p_open) {
	ImGuiIO io = ImGui::GetIO();
	ImFont* MainMenuFont = io.Fonts->Fonts[1];
	ImFont* ButtonFont = io.Fonts->Fonts[2];

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;

	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();							// For setting the main menu window to fullscreen
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	float w = ImGui::GetWindowWidth();
	const ImVec2 padding = ImVec2(24.f, 24.f);
	const ImVec2 button_size = ImVec2(w, ButtonFont->FontSize + 20.0f);
	const float text_center = (w - ImGui::CalcTextSize("Aria: Whipsers Of Darkness").x) * 0.5f;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.084f, 0.067f, 0.148f, 1.0f));		// Set Window Background Color
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);							// Set Window Padding
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.0f);						    // Set Frame Rounding

	if (ImGui::Begin("Main Menu", p_open, flags))
	{
		ImVec2 spc = ImVec2(0.f, 60.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

		ImGui::PushFont(MainMenuFont);
		ImGui::SetCursorPosX(text_center);
		ImGui::Text("Aria: Whipsers Of Darkness");
		ImGui::PopFont();
		ImGui::PopStyleVar();

		spc = ImVec2(0.f, 20.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

		ImGui::PushFont(ButtonFont);
		if (ImGui::Button("Start Game", button_size)) {
			state = GAME_START;
			*p_open = false;
		}

		if (ImGui::Button("Load Game", button_size)) {
			state = LOAD;
			//*p_open = false;
		}

		if (ImGui::Button("Quit Game", button_size)) {
			state = GAME_OVER;
			*p_open = false;
		}

		ImGui::PopStyleVar();

		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}