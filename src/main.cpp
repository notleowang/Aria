#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "ai_system.hpp"
#include "main_menu.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world_system;
	RenderSystem render_system;
	PhysicsSystem physics_system;
	AISystem ai_system;

	// Window states
	bool show_main_menu = true;

	// Initializing window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems below
	// initialize the render system
	render_system.init(window);

	// Initialize the level for the world system
	GameLevel level;
	level.init(TUTORIAL);

	world_system.init(&render_system, level);
	ai_system.init(&render_system);

	// variable timestep loop
	auto t = Clock::now();
	while (!world_system.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		if (show_main_menu) {

		}

		world_system.step(elapsed_ms);
		physics_system.step(elapsed_ms);
		ai_system.step(elapsed_ms);

		world_system.handle_collisions();

		render_system.animation_step(elapsed_ms);
		render_system.draw();
	}

	return EXIT_SUCCESS;
}
