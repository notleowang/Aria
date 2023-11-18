#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;  // ID handle of the glyph texture
	ivec2   Size;      // Size of glyph
	ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("salmon.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("fish.png"),   // Currently keeping this so we know how to import our textures
			textures_path("landscape.png"),
			textures_path("water_enemy.png"),
			textures_path("fire_enemy.png") ,
			textures_path("earth_enemy.png"),
			textures_path("lightning_enemy.png"),
			textures_path("ghost.png"),
			textures_path("projectile_water_sprite_sheet.png"),
			textures_path("projectile_fire_sprite_sheet.png"),
			textures_path("projectile_earth.png"),
			textures_path("projectile_lightning.png"),
			textures_path("dungeon_tile.png"),
			textures_path("health_bar.png"),
			textures_path("mana_bar.png"),
			textures_path("power_up_block.png"),
			textures_path("witch.png"),
			textures_path("portal.png")
			textures_path("projectile-select-display-purple.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("aria"),
		shader_path("coloured"),
		shader_path("salmon"),
		shader_path("textured"),
		shader_path("screen_darken"),
		shader_path("terrain"),
		shader_path("exit_door"),
		shader_path("resource_bar"),
		shader_path("text_2d"),
		shader_path("animated"),
		shader_path("shadow")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

	std::array<SpriteSheet, sprite_sheet_count> sprite_sheets;

	GLuint vao;
	std::unordered_map<GLchar, Character> Characters;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeSpriteSheets();
	SpriteSheet& getSpriteSheet(SPRITE_SHEET_DATA_ID id) { return sprite_sheets[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water
	// shader
	bool initScreenTexture();

	void initializeFreeType();

	void initializeImGui();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	void animation_step(float elapsed_ms);

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();
	void drawText(Entity entity);
	void drawImGui();
	void drawArsenal(Entity entity, const mat3& projection);

	// Helper functions for initializeSpriteSheets()
	void initializePowerUpBlockSpriteSheet();
	void initializeProjectileSpriteSheet(SPRITE_SHEET_DATA_ID ss_id, int num_cols);
	void initializePlayerSpriteSheet();
	void initializeProjectileSelectDisplaySpriteSheet();

	// Helper functions for initializeGlGeometryBuffers()
	void initializePlayerGeometryBuffer();
	void initializeSpriteGeometryBuffer();
	void initializeDebugLineGeometryBuffer();
	void initializeScreenTriangleGeometryBuffer();
	void initializeTerrainGeometryBuffer();
	void initializeExitDoorGeometryBuffer();
	void initializeResourceBarGeometryBuffer();
	void initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID goem_buffer_id, SPRITE_SHEET_DATA_ID ss_id);

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;

	float elapsed_time = 0.f;
	const float ANIMATION_SPEED = 100.f;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
