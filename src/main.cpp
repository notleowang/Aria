#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "ai_system.hpp"
#include "ui_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world_system;
	RenderSystem render_system;
	PhysicsSystem physics_system;
	AISystem ai_system;

	// UI system
	UISystem ui_system;

	// Window states
	bool show_main_menu = true;

	UISystem::State state;

	// Initializing window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the render system
	render_system.init(window);

	// initialize the level for the world system
	GameLevel level;
	level.init(TUTORIAL);

	// initialize other main systems
	world_system.init(&render_system, level);
	ai_system.init(&render_system);

	// variable timestep loop
	auto t = Clock::now();
	while (!world_system.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// initialize UI system
		ui_system.init();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		if (show_main_menu) {
			ui_system.showWindows();
			state = ui_system.getState();
			//ImGui::ShowDemoWindow();
		}

		if (state == 0) {
			world_system.step(elapsed_ms);
			physics_system.step(elapsed_ms);
			ai_system.step(elapsed_ms);

			world_system.handle_collisions();
		}

		if (state == 4) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		render_system.animation_step(elapsed_ms);
		render_system.draw();
	}

	return EXIT_SUCCESS;
}
