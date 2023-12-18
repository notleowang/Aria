// internal
#include "render_system.hpp"

#include <array>
#include <fstream>

#include "../ext/stb_image/stb_image.h"

// This creates circular header inclusion, that is quite bad.
#include "tiny_ecs_registry.hpp"

// stlib
#include <iostream>
#include <sstream>

// define our font path
#ifdef __linux__
	#define FONT_PATH "data/fonts/PixeloidSans.ttf"
#else
	#define FONT_PATH "../../../data/fonts/PixeloidSans.ttf"
#endif

// World initialization
bool RenderSystem::init(GLFWwindow* window_arg)
{
	this->window = window_arg;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	const int is_fine = gl3w_init();
	assert(is_fine == 0);

	// Create a frame buffer
	frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int frame_buffer_width_px, frame_buffer_height_px;
	glfwGetFramebufferSize(window, &frame_buffer_width_px, &frame_buffer_height_px);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	if (frame_buffer_width_px != window_width_px)
	{
		printf("WARNING: retina display! https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value\n");
		printf("glfwGetFramebufferSize = %d,%d\n", frame_buffer_width_px, frame_buffer_height_px);
		printf("window width_height = %d,%d\n", window_width_px, window_height_px);
	}

	// Hint: Ask your TA for how to setup pretty OpenGL error callbacks. 
	// This can not be done in mac os, so do not enable
	// it unless you are on Linux or Windows. You will need to change the window creation
	// code to use OpenGL 4.3 (not suported on mac) and add additional .h and .cpp
	// glDebugMessageCallback((GLDEBUGPROC)errorCallback, nullptr);

	// We are not really using VAO's but without at least one bound we will crash in
	// some systems.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	gl_has_errors();

	initScreenTexture();
    initializeGlTextures();
	initializeGlEffects();
	initializeSpriteSheets(); // must be called before initializeGlGeometryBuffers()
	initializeGlGeometryBuffers();
	initializeImGui();
	initializeFreeType();

	return true;
}

void RenderSystem::initializeFreeType() {
	// FreeType Reference: https://learnopengl.com/In-Practice/Text-Rendering
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, FONT_PATH, 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	else {
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)GEOMETRY_BUFFER_ID::TEXT_2D]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	gl_has_errors();
}

void RenderSystem::initializeImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Fonts
	ImFont* DefaultFont = io.Fonts->AddFontFromFileTTF(FONT_PATH, 24.0f);	// index 0
	ImFont* MainMenuFont = io.Fonts->AddFontFromFileTTF(FONT_PATH, 60.0f); // index 1
	ImFont* MainMenuButtonFont = io.Fonts->AddFontFromFileTTF(FONT_PATH, 48.0f); // index 1

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void RenderSystem::initializeGlTextures()
{
    glGenTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());

    for(uint i = 0; i < texture_paths.size(); i++)
    {
		const std::string& path = texture_paths[i];
		ivec2& dimensions = texture_dimensions[i];

		stbi_uc* data;
		data = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

		if (data == NULL)
		{
			const std::string message = "Could not load the file " + path + ".";
			fprintf(stderr, "%s", message.c_str());
			assert(false);
		}
		glBindTexture(GL_TEXTURE_2D, texture_gl_handles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl_has_errors();
		stbi_image_free(data);
    }
}

void RenderSystem::initializeGlEffects()
{
	for(uint i = 0; i < effect_paths.size(); i++)
	{
		const std::string vertex_shader_name = effect_paths[i] + ".vs.glsl";
		const std::string fragment_shader_name = effect_paths[i] + ".fs.glsl";

		bool is_valid = loadEffectFromFile(vertex_shader_name, fragment_shader_name, effects[i]);
		assert(is_valid && (GLuint)effects[i] != 0);
	}
}

// One could merge the following two functions as a template function...
template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

void RenderSystem::initializeGlMeshes()
{
	for (uint i = 0; i < mesh_paths.size(); i++)
	{
		// Initialize meshes
		GEOMETRY_BUFFER_ID geom_index = mesh_paths[i].first;
		std::string name = mesh_paths[i].second;
		Mesh::loadFromOBJFile(name, 
			meshes[(int)geom_index].vertices,
			meshes[(int)geom_index].vertex_indices,
			meshes[(int)geom_index].original_size);

		bindVBOandIBO(geom_index,
			meshes[(int)geom_index].vertices, 
			meshes[(int)geom_index].vertex_indices);
	}
}

void RenderSystem::initializePowerUpBlockSpriteSheet()
{
	int num_rows = 4;
	int num_cols = 5;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::POWER_UP_BLOCK;

	std::vector<AnimState> states((int)POWER_UP_BLOCK_STATES::STATE_COUNT);
	states[(int)POWER_UP_BLOCK_STATES::ACTIVE] = AnimState(0, 13);
	states[(int)POWER_UP_BLOCK_STATES::INCREASE_DAMAGE] = AnimState(15, 15);
	states[(int)POWER_UP_BLOCK_STATES::TRIPLE_SHOT] = AnimState(16, 16);
	states[(int)POWER_UP_BLOCK_STATES::BOUNCE_SHOT] = AnimState(17, 17);
	states[(int)POWER_UP_BLOCK_STATES::FASTER_MOVEMENT] = AnimState(18, 18);
	states[(int)POWER_UP_BLOCK_STATES::EMPTY_LIGHT] = AnimState(14, 14);
	states[(int)POWER_UP_BLOCK_STATES::EMPTY_DARK] = AnimState(19, 19);

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
	sprite_sheets[ss_index].frame_height = 32.f;
	sprite_sheets[ss_index].frame_width = 32.f;
}

void RenderSystem::initializeProjectileSpriteSheet(SPRITE_SHEET_DATA_ID ss_id, int num_cols, float frame_height, float frame_width)
{
	int num_rows = 1;
	int ss_index = (int)ss_id;

	std::vector<AnimState> states((int)PROJECTILE_STATES::STATE_COUNT);
	states[(int)PROJECTILE_STATES::MOVING] = AnimState(0, num_cols - 1);

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
	sprite_sheets[ss_index].frame_height = frame_height;
	sprite_sheets[ss_index].frame_width = frame_width;
}

void RenderSystem::initializePlayerSpriteSheet()
{
	int num_rows = 6;
	int num_cols = 4;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::PLAYER;
	int num_states = (int)PLAYER_SPRITE_STATES::STATE_COUNT;
	//int num_frames_in_state = num_cols / num_states;
	int num_frames_in_state = 4;

	std::vector<AnimState> states(num_states);
	for (int i = 0; i < num_states; i++) {
		states[i] = AnimState(i * num_frames_in_state, (i + 1) * num_frames_in_state - 1);
	}

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
}

void RenderSystem::initializeLostSoulSpriteSheet()
{
	int num_rows = 6;
	int num_cols = 4;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::LOST_SOUL;
	int num_states = (int)LOST_SOUL_STATES::STATE_COUNT;

	std::vector<AnimState> states(num_states);
	for (int i = 0; i < num_rows; i++) {
		states[i] = AnimState(i * num_cols, (i + 1) * num_cols - 1);
	}
	states[(int)LOST_SOUL_STATES::EAST_STILL] = AnimState(13.f, 13.f);
	states[(int)LOST_SOUL_STATES::WEST_STILL] = AnimState(17.f, 17.f);
	states[(int)LOST_SOUL_STATES::SOUTH_STILL] = AnimState(21.f, 21.f);
  
  sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
  sprite_sheets[ss_index].frame_height = 31.f;
	sprite_sheets[ss_index].frame_width = 31.f;
}

void RenderSystem::initializeBossSpriteSheet()
{
	int num_rows = 1;
	int num_cols = 8;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::BOSS;

	std::vector<AnimState> states((int)BOSS_STATES::STATE_COUNT);
	states[(int)BOSS_STATES::STANDING] = AnimState(0, num_cols-1);

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
	sprite_sheets[ss_index].frame_height = 70.f;
	sprite_sheets[ss_index].frame_width = 56.f;
}

void RenderSystem::initializeFinalBossSpriteSheet()
{
	int num_rows = 3;
	int num_cols = 9;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::FINAL_BOSS;

	/*
	* Final boss sprite sheet frames:
	* 
	* | 0-2   | 3-5   |	xxx     | 	EAST
	* | 9-11  | 12-14 |	xxx     | 	WEST
	* | 18-20 | 21-23 |	24-26   | 	SOUTH
	* 
	* NOTE: x's are blanks
	*/
	std::vector<AnimState> states((int)FINAL_BOSS_SPRITE_STATES::STATE_COUNT);
	states[(int)FINAL_BOSS_SPRITE_STATES::EAST] = AnimState(0, 5);
	states[(int)FINAL_BOSS_SPRITE_STATES::WEST] = AnimState(9, 14);
	states[(int)FINAL_BOSS_SPRITE_STATES::SOUTH] = AnimState(18, 26);

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
	sprite_sheets[ss_index].frame_height = 324.f / num_rows;
	sprite_sheets[ss_index].frame_width = 648.f / num_cols;
}

void RenderSystem::initializeFinalBossAuraSpriteSheet()
{
	int num_rows = 1;
	int num_cols = 5;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::FINAL_BOSS_AURA;

	std::vector<AnimState> states((int)FINAL_BOSS_AURA_SPRITE_STATES::STATE_COUNT);
	for (int i = 0; i < (int)FINAL_BOSS_AURA_SPRITE_STATES::STATE_COUNT; i++) {
		states[i] = AnimState(i, i);
	}

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
	sprite_sheets[ss_index].frame_height = 128.f;
	sprite_sheets[ss_index].frame_width = 128.f;
}

void RenderSystem::initializeProjectileSelectDisplaySpriteSheet()
{
	int num_rows = 1;
	int num_cols = 4;
	int ss_index = (int)SPRITE_SHEET_DATA_ID::PROJECTILE_SELECT_DISPLAY;

	std::vector<AnimState> states((int)ElementType::COUNT);
	for (int i = 0; i < (int)ElementType::COUNT; i++) {
		states[i] = AnimState(i, i);
	}

	sprite_sheets[ss_index].num_rows = num_rows;
	sprite_sheets[ss_index].num_cols = num_cols;
	sprite_sheets[ss_index].states = states;
	sprite_sheets[ss_index].frame_height = 123.f;
	sprite_sheets[ss_index].frame_width = 33.f;
}

void RenderSystem::initializeSpriteSheets()
{
	initializePowerUpBlockSpriteSheet();
	initializeProjectileSpriteSheet(SPRITE_SHEET_DATA_ID::WATER_PROJECTILE, 4, 16.f, 26.f);
	initializeProjectileSpriteSheet(SPRITE_SHEET_DATA_ID::FIRE_PROJECTILE, 4, 16.f, 26.f);
	initializeProjectileSpriteSheet(SPRITE_SHEET_DATA_ID::EARTH_PROJECTILE_SHEET, 6, 16.f, 26.f);
	initializeProjectileSpriteSheet(SPRITE_SHEET_DATA_ID::LIGHTNING_PROJECTILE_SHEET, 6, 16.f, 26.f);
	initializePlayerSpriteSheet();
	initializeLostSoulSpriteSheet();
	initializeBossSpriteSheet();
	initializeFinalBossSpriteSheet();
	initializeFinalBossAuraSpriteSheet();
	initializeProjectileSelectDisplaySpriteSheet();
}

// Helper functions for initializing Gl Geometry Buffers
void RenderSystem::initializePlayerGeometryBuffer()
{
	int geom_index = (int)GEOMETRY_BUFFER_ID::PLAYER;
	int ss_id = (int)SPRITE_SHEET_DATA_ID::PLAYER;
	int num_rows = sprite_sheets[(int)ss_id].num_rows;
	int num_cols = sprite_sheets[(int)ss_id].num_cols;

	// square aspect ratio
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { 0.2f,				1.f / num_rows };
	textured_vertices[1].texcoord = { 1.f / num_cols - 0.2f,	1.f / num_rows };
	textured_vertices[2].texcoord = { 1.f / num_cols - 0.2f,	0.01f };
	textured_vertices[3].texcoord = { 0.2f,				0.01f };

	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };

	std::vector<ColoredVertex> vertices(4);
	vertices[0].position = textured_vertices[0].position;
	vertices[1].position = textured_vertices[1].position;
	vertices[2].position = textured_vertices[2].position;
	vertices[3].position = textured_vertices[3].position;

	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = textured_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::PLAYER, textured_vertices, textured_indices);
}

void RenderSystem::initializeSmallEnemyGeometryBuffer()
{
	static const std::vector<GEOMETRY_BUFFER_ID> enemies = { GEOMETRY_BUFFER_ID::SMALL_WATER_ENEMY, 
		GEOMETRY_BUFFER_ID::SMALL_FIRE_ENEMY, 
		GEOMETRY_BUFFER_ID::SMALL_EARTH_ENEMY, 
		GEOMETRY_BUFFER_ID::SMALL_LIGHTNING_ENEMY };
	for (uint i = 0; i < enemies.size(); i++) {
		int geom_index = (int)enemies[i];
		float left_x_cutoff = 0.f;
		float right_x_cutoff = 0.f;
		float top_y_cutoff = 0.f;
		float bottom_y_cutoff = 0.f;
		switch ((GEOMETRY_BUFFER_ID)geom_index) {
			case (GEOMETRY_BUFFER_ID::SMALL_WATER_ENEMY):
				left_x_cutoff = 0.12f;
				right_x_cutoff = 0.1f;
				top_y_cutoff = 0.17f;
				bottom_y_cutoff = 0.07f;
				break;			
			case (GEOMETRY_BUFFER_ID::SMALL_FIRE_ENEMY):
				left_x_cutoff = 0.12f;
				right_x_cutoff = 0.10f;
				top_y_cutoff = 0.10f;
				bottom_y_cutoff = 0.15f;
				break;			
			case (GEOMETRY_BUFFER_ID::SMALL_EARTH_ENEMY):
				left_x_cutoff = 0.09f;
				right_x_cutoff = 0.07f;
				top_y_cutoff = 0.15f;
				bottom_y_cutoff = 0.15f;
				break;
			case (GEOMETRY_BUFFER_ID::SMALL_LIGHTNING_ENEMY):
				left_x_cutoff = 0.18f;
				right_x_cutoff = 0.12f;
				top_y_cutoff = 0.21f;
				bottom_y_cutoff = 0.23f;
				break;
			default:
				break;
			
		}
		std::vector<TexturedVertex> textured_vertices(4);
		textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
		textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
		textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
		textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
		textured_vertices[0].texcoord = { left_x_cutoff, 1-top_y_cutoff};
		textured_vertices[1].texcoord = { 1-right_x_cutoff, 1-top_y_cutoff};
		textured_vertices[2].texcoord = { 1-right_x_cutoff, bottom_y_cutoff};
		textured_vertices[3].texcoord = { left_x_cutoff, bottom_y_cutoff };

		// Counterclockwise as it's the default opengl front winding direction.
		const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };

		std::vector<ColoredVertex> vertices(4);
		vertices[0].position = textured_vertices[0].position;
		vertices[1].position = textured_vertices[1].position;
		vertices[2].position = textured_vertices[2].position;
		vertices[3].position = textured_vertices[3].position;

		meshes[geom_index].vertices = vertices;
		meshes[geom_index].vertex_indices = textured_indices;
		bindVBOandIBO((GEOMETRY_BUFFER_ID)geom_index, textured_vertices, textured_indices);
	}
}

void RenderSystem::initializeSpriteGeometryBuffer()
{
	int geom_index = (int)GEOMETRY_BUFFER_ID::SPRITE;

	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { 0.f, 1.f };
	textured_vertices[1].texcoord = { 1.f, 1.f };
	textured_vertices[2].texcoord = { 1.f, 0.f };
	textured_vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };

	std::vector<ColoredVertex> vertices(4);
	vertices[0].position = textured_vertices[0].position;
	vertices[1].position = textured_vertices[1].position;
	vertices[2].position = textured_vertices[2].position;
	vertices[3].position = textured_vertices[3].position;

	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = textured_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, textured_vertices, textured_indices);
}

void RenderSystem::initializeDebugLineGeometryBuffer()
{
	int geom_index = (int)GEOMETRY_BUFFER_ID::DEBUG_LINE;

	std::vector<ColoredVertex> vertices(4);
	std::vector<uint16_t> vertex_indices;

	constexpr float depth = 0.5f;
	constexpr vec3 red = { 0.8,0.1,0.1 };

	// Corner points
	vertices = {
		{{-0.5,-0.5, depth}, red},
		{{-0.5, 0.5, depth}, red},
		{{ 0.5, 0.5, depth}, red},
		{{ 0.5,-0.5, depth}, red},
	};

	// Two triangles
	vertex_indices = { 0, 1, 3, 1, 2, 3 };

	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = vertex_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::DEBUG_LINE, vertices, vertex_indices);
}

void RenderSystem::initializeScreenTriangleGeometryBuffer()
{
	std::vector<vec3> vertices(3);
	vertices[0] = { -1, -6, 0.f };
	vertices[1] = { 6, -1, 0.f };
	vertices[2] = { -1, 6, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> vertex_indices = { 0, 1, 2 };

	// No mesh for the screen triangle
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE, vertices, vertex_indices);
}

void RenderSystem::initializeTerrainGeometryBuffer()
{
	int geom_index = (int)GEOMETRY_BUFFER_ID::TERRAIN;

	std::vector<ColoredVertex> vertices(4);
	vertices[0].position = { -0.5, -0.5, -0.1 };
	vertices[1].position = { -0.5, 0.5, -0.1 };
	vertices[2].position = { 0.5, 0.5, -0.1 };
	vertices[3].position = { 0.5, -0.5, -0.1 };

	const std::vector<uint16_t> vertex_indices = { 0, 1, 2, 0, 2, 3 };
	
	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = vertex_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::TERRAIN, meshes[geom_index].vertices, meshes[geom_index].vertex_indices);
}

void RenderSystem::initializeExitDoorGeometryBuffer()
{
	int geom_index = (int)GEOMETRY_BUFFER_ID::EXIT_DOOR;

	std::vector<ColoredVertex> vertices(4);
	vertices[0].position = { -0.5, -0.5, -0.1 };
	vertices[1].position = { -0.5, 0.5, -0.1 };
	vertices[2].position = { 0.5, 0.5, -0.1 };
	vertices[3].position = { 0.5, -0.5, -0.1 };

	const std::vector<uint16_t> vertex_indices = { 0, 1, 2, 0, 2, 3 };

	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = vertex_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::EXIT_DOOR, meshes[geom_index].vertices, meshes[geom_index].vertex_indices);
}

void RenderSystem::initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID geom_buffer_id, SPRITE_SHEET_DATA_ID ss_id)
{
	int geom_index = (int)geom_buffer_id;
	int num_rows = sprite_sheets[(int)ss_id].num_rows;
	int num_cols = sprite_sheets[(int)ss_id].num_cols;

	// square aspect ratio
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { 0.f,				1.f / num_rows };
	textured_vertices[1].texcoord = { 1.f / num_cols,	1.f / num_rows };
	textured_vertices[2].texcoord = { 1.f / num_cols,	0.f };
	textured_vertices[3].texcoord = { 0.f,				0.f };

	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };

	std::vector<ColoredVertex> vertices(4);
	vertices[0].position = textured_vertices[0].position;
	vertices[1].position = textured_vertices[1].position;
	vertices[2].position = textured_vertices[2].position;
	vertices[3].position = textured_vertices[3].position;

	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = textured_indices;
	bindVBOandIBO(geom_buffer_id, textured_vertices, textured_indices);
}

void RenderSystem::initializeResourceBarGeometryBuffer()
{
	int geom_index = (int)GEOMETRY_BUFFER_ID::RESOURCE_BAR;

	// Initial texture coords are centered on empty health bar
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { 0.f, 1.f / 2 };
	textured_vertices[1].texcoord = { 1.f, 1.f / 2 };
	textured_vertices[2].texcoord = { 1.f, 0.f };
	textured_vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };

	std::vector<ColoredVertex> vertices(4);
	vertices[0].position = textured_vertices[0].position;
	vertices[1].position = textured_vertices[1].position;
	vertices[2].position = textured_vertices[2].position;
	vertices[3].position = textured_vertices[3].position;

	meshes[geom_index].vertices = vertices;
	meshes[geom_index].vertex_indices = textured_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::RESOURCE_BAR, textured_vertices, textured_indices);
}

void RenderSystem::initializeGlGeometryBuffers()
{
	// Vertex Buffer creation.
	glGenBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	// Index Buffer creation.
	glGenBuffers((GLsizei)index_buffers.size(), index_buffers.data());

	// Index and Vertex buffer data initialization.
	initializeGlMeshes();

	initializeSpriteGeometryBuffer();
	initializeDebugLineGeometryBuffer();
	initializeScreenTriangleGeometryBuffer();
	initializeTerrainGeometryBuffer();
	initializeExitDoorGeometryBuffer();
	// function initializeSpriteSheets must be called before this point
	initializePlayerGeometryBuffer();
	initializeSmallEnemyGeometryBuffer();
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::FIRE_PROJECTILE, SPRITE_SHEET_DATA_ID::FIRE_PROJECTILE);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::WATER_PROJECTILE, SPRITE_SHEET_DATA_ID::WATER_PROJECTILE);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::EARTH_PROJECTILE_SHEET, SPRITE_SHEET_DATA_ID::EARTH_PROJECTILE_SHEET);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::LIGHTNING_PROJECTILE_SHEET, SPRITE_SHEET_DATA_ID::LIGHTNING_PROJECTILE_SHEET);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::POWER_UP_BLOCK, SPRITE_SHEET_DATA_ID::POWER_UP_BLOCK);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::PROJECTILE_SELECT_DISPLAY, SPRITE_SHEET_DATA_ID::PROJECTILE_SELECT_DISPLAY);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::LOST_SOUL, SPRITE_SHEET_DATA_ID::LOST_SOUL);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::BOSS, SPRITE_SHEET_DATA_ID::BOSS);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::FINAL_BOSS, SPRITE_SHEET_DATA_ID::FINAL_BOSS);
	initializeSpriteSheetGeometryBuffer(GEOMETRY_BUFFER_ID::FINAL_BOSS_AURA, SPRITE_SHEET_DATA_ID::FINAL_BOSS_AURA);
	initializeResourceBarGeometryBuffer();
}

RenderSystem::~RenderSystem()
{
	// Don't need to free gl resources since they last for as long as the program,
	// but it's polite to clean after yourself.
	glDeleteBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	glDeleteBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	glDeleteTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());
	glDeleteTextures(1, &off_screen_render_buffer_color);
	glDeleteRenderbuffers(1, &off_screen_render_buffer_depth);
	gl_has_errors();

	for(uint i = 0; i < effect_count; i++) {
		glDeleteProgram(effects[i]);
	}
	// delete allocated resources
	glDeleteFramebuffers(1, &frame_buffer);
	gl_has_errors();

	// remove all entities created by the render system
	while (registry.renderRequests.entities.size() > 0)
	    registry.remove_all_components_of(registry.renderRequests.entities.back());
}

// Initialize the screen texture from a standard sprite
bool RenderSystem::initScreenTexture()
{
	registry.screenStates.emplace(screen_state_entity);

	int framebuffer_width, framebuffer_height;
	glfwGetFramebufferSize(const_cast<GLFWwindow*>(window), &framebuffer_width, &framebuffer_height);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	glGenTextures(1, &off_screen_render_buffer_color);
	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl_has_errors();

	glGenRenderbuffers(1, &off_screen_render_buffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, off_screen_render_buffer_depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, off_screen_render_buffer_color, 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, off_screen_render_buffer_depth);
	gl_has_errors();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

bool gl_compile_shader(GLuint shader)
{
	glCompileShader(shader);
	gl_has_errors();
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		gl_has_errors();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program)
{
	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);
	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path.c_str(), fs_path.c_str());
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);
	gl_has_errors();

	// Compiling
	if (!gl_compile_shader(vertex))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if (!gl_compile_shader(fragment))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}

	// Linking
	out_program = glCreateProgram();
	glAttachShader(out_program, vertex);
	glAttachShader(out_program, fragment);
	glLinkProgram(out_program);
	gl_has_errors();

	{
		GLint is_linked = GL_FALSE;
		glGetProgramiv(out_program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(out_program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(out_program, log_len, &log_len, log.data());
			gl_has_errors();

			fprintf(stderr, "Link error: %s", log.data());
			assert(false);
			return false;
		}
	}

	// No need to carry this around. Keeping these objects is only useful if we recycle
	// the same shaders over and over, which we don't, so no need and this is simpler.
	glDetachShader(out_program, vertex);
	glDetachShader(out_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_has_errors();

	return true;
}

