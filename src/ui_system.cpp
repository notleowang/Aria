#include "ui_system.hpp"

UISystem* UISystem::instancePtr = NULL;

void UISystem::init() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UISystem::showWindows() {
	static bool show_menu = true;
	bool show_tutorial = isTutorial;

	if (state == MAIN_MENU) showMainMenu(&show_menu);
	if (state == PAUSE_MENU) showPauseMenu(&show_menu);
	if (show_tutorial) showTutorial(&show_tutorial);
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

	//float w = ImGui::GetWindowWidth(); For some reason this is returning 400.f? Ask TA maybe? - Leo
	float w = viewport->Size.x;
	const ImVec2 padding = ImVec2(24.f, 24.f);
	const ImVec2 button_size = ImVec2(350.f, ButtonFont->FontSize + 20.0f);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.084f, 0.067f, 0.148f, 1.0f));		// Set Window Background Color
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);							// Set Window Padding
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.0f);						    // Set Frame Rounding

	if (ImGui::Begin("Main Menu", p_open, flags))
	{
		ImVec2 spc = ImVec2(0.f, 100.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

		ImGui::PushFont(MainMenuFont);
		ImGui::SetCursorPosY(200.f);
		CenterText("Aria: Whispers Of Darkness");
		ImGui::PopFont();
		ImGui::PopStyleVar();

		spc = ImVec2(0.f, 20.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

		ImGui::PushFont(ButtonFont);
		ImGui::SetCursorPosX((w - button_size.x) * 0.5f);
		if (ImGui::Button("Start Game", button_size)) {
			state = NEW_GAME;
			*p_open = false;
		}

		// TODO: uncomment when implementing reloadability
		//ImGui::SetCursorPosX((w - button_size.x) * 0.5f);
		//if (ImGui::Button("Save Game", button_size)) {
		//	state = SAVE;
		//	//*p_open = false;
		//}

		//ImGui::SetCursorPosX((w - button_size.x) * 0.5f);
		//if (ImGui::Button("Load Game", button_size)) {
		//	state = LOAD;
		//	//*p_open = false;
		//}

		ImGui::SetCursorPosX((w - button_size.x) * 0.5f);
		if (ImGui::Button("Quit Game", button_size)) {
			state = QUIT;
			*p_open = false;
		}

		ImGui::PopStyleVar();
		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void UISystem::showPauseMenu(bool* p_open) {


	ImGuiIO io = ImGui::GetIO();
	ImFont* MainMenuFont = io.Fonts->Fonts[1];
	ImFont* ButtonFont = io.Fonts->Fonts[2];

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;

	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();							// For setting the pause menu window to fullscreen
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	//float w = ImGui::GetWindowWidth(); For some reason this is returning 400.f? Ask TA maybe? - Leo
	float w = viewport->Size.x;
	const ImVec2 padding = ImVec2(24.f, 24.f);
	const ImVec2 button_size = ImVec2(350.f, ButtonFont->FontSize + 20.0f);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.084f, 0.067f, 0.148f, 1.0f));		// Set Window Background Color
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);							// Set Window Padding
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.0f);						    // Set Frame Rounding

	if (ImGui::Begin("Pause Menu", p_open, flags))
	{
		ImVec2 spc = ImVec2(0.f, 100.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

		ImGui::PushFont(MainMenuFont);
		ImGui::SetCursorPosY(150.f);
		CenterText("Options");
		ImGui::PopFont();
		ImGui::PopStyleVar();

		spc = ImVec2(0.f, 20.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

		ImGui::PushFont(ButtonFont);
		ImGui::SetCursorPosX((w - button_size.x) * 0.5f);
		if (ImGui::Button("Resume Game", button_size)) {
			state = PLAY_GAME;
			*p_open = false;
		}
		
		//ImGui::SetCursorPosX((w - 1000.f) * 0.5f);
		//if (ImGui::SliderInt("Volume", &volume, 0, MIX_MAX_VOLUME)) {
		//	Mix_VolumeMusic(volume); // set background music volume
		//	Mix_Volume(-1, volume); // set sound effects volume
		//}

		ImGui::SetCursorPosX((w - button_size.x) * 0.5f);
		if (ImGui::Button("Exit Level", button_size)) {
			state = MAIN_MENU;
			*p_open = false;
		}

		ImGui::PopStyleVar();
		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void UISystem::showTutorial(bool* p_open) {
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;

	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
	
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if (ImGui::Begin("Tutorial", p_open, flags)) {

		//WorldCoordinateText("Welcome to Aria: Whispers of Darkness!", 40, 40);
		//ImGui::Text("This is a tutorial on how to play the game.");
		//ImGui::Text("The goal of the game is to defeat the boss at the end of the level.");
		//ImGui::Text("You can move around with WASD and use your mouse to aim.");
		//ImGui::Text("You can attack with your left mouse button and dodge with your right mouse button.");
		//ImGui::Text("You can also use your abilities with the number keys 1-4.");
		//ImGui::Text("You can pause the game with the escape key.");
		//ImGui::Text("Good luck!");
	}

	ImGui::End();
}

void UISystem::CenterText(const char* text) {
	ImVec2 textSize = ImGui::CalcTextSize(text);
	float w = ImGui::GetWindowWidth();
	float center = (w - textSize.x) * 0.5f;

	// Center the text horizontally
	ImGui::SetCursorPosX(center);

	// Draw the centered text
	ImGui::Text(text);
}

void UISystem::WorldCoordinateText(const char* text, float x, float y) {
	Entity player = registry.players.entities[0];
	vec2 player_pos = registry.positions.get(player).position;
	float left = -(player_pos.x - (float)window_width_px / 2);
	float top = -(player_pos.y - (float)window_height_px / 2);

	// Position the text at the given coordinates
	ImGui::SetCursorPos(ImVec2(left + x, top + y));

	// Draw the text
	ImGui::Text(text);
}