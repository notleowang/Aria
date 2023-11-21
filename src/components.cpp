#include "components.hpp"
#include "render_system.hpp" // for gl_has_errors

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

// stlib
#include <iostream>
#include <sstream>

Debug debugging;
float death_timer_timer_ms = 3000;

// Very, VERY simple OBJ loader from https://github.com/opengl-tutorials/ogl tutorial 7
// (modified to also read vertex color and omit uv and normals)
bool Mesh::loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size)
{
	// disable warnings about fscanf and fopen on Windows
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

	printf("Loading OBJ file %s...\n", obj_path.c_str());
	// Note, normal and UV indices are not loaded/used, but code is commented to do so
	std::vector<uint16_t> out_uv_indices, out_normal_indices;
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;

	FILE* file = fopen(obj_path.c_str(), "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0) {
			ColoredVertex vertex;
			int matches = fscanf(file, "%f %f %f %f %f %f\n", &vertex.position.x, &vertex.position.y, &vertex.position.z,
				&vertex.color.x, &vertex.color.y, &vertex.color.z);
			if (matches == 3)
				vertex.color = { 1,1,1 };
			out_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			out_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			out_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];

			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches == 1) // try again
			{
				// Note first vertex index is already consumed by the first fscanf call (match ==1) since it aborts on the first error
				matches = fscanf(file, "//%d %d//%d %d//%d\n", &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 5) // try again
				{
					matches = fscanf(file, "%d/%d %d/%d/%d %d/%d/%d\n", &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 8)
					{
						printf("File can't be read by our simple parser :-( Try exporting with other options\n");
						fclose(file);
						return false;
					}
				}
			}

			// -1 since .obj starts counting at 1 and OpenGL starts at 0
			out_vertex_indices.push_back((uint16_t)vertexIndex[0] - 1);
			out_vertex_indices.push_back((uint16_t)vertexIndex[1] - 1);
			out_vertex_indices.push_back((uint16_t)vertexIndex[2] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[0] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[1] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[2] - 1);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	fclose(file);

	// Compute bounds of the mesh
	vec3 max_position = { -99999,-99999,-99999 };
	vec3 min_position = { 99999,99999,99999 };
	for (ColoredVertex& pos : out_vertices)
	{
		max_position = glm::max(max_position, pos.position);
		min_position = glm::min(min_position, pos.position);
	}
	if(abs(max_position.z - min_position.z)<0.001)
		max_position.z = min_position.z+1; // don't scale z direction when everythin is on one plane

	vec3 size3d = max_position - min_position;
	out_size = size3d;

	// Normalize mesh to range -0.5 ... 0.5
	for (ColoredVertex& pos : out_vertices)
		pos.position = ((pos.position - min_position) / size3d) - vec3(0.5f, 0.5f, 0.5f);

	return true;
}

int AnimState::getNumFrames()
{
	return (last + 1) - first;
}

int AnimState::getNextFrame(int curr_frame)
{
	if (curr_frame < first || curr_frame > last) {
		return first;
	}
	return ((curr_frame - first + 1) % getNumFrames()) + first;
}

vec2 SpriteSheet::getFrameSizeInTexcoords()
{
	return vec2(1.f / num_cols, 1.f / num_rows);
}

int SpriteSheet::getNumFrames()
{
	return num_cols * num_rows;
}

int Animation::getColumn()
{
	return curr_frame % sprite_sheet_ptr->num_cols;
}

int Animation::getRow()
{
	return floor((float)curr_frame / sprite_sheet_ptr->num_cols);
}

void Animation::advanceFrame()
{
	AnimState state = sprite_sheet_ptr->states[curr_state_index];
	curr_frame = state.getNextFrame(curr_frame);
}

void Animation::advanceState()
{
	curr_state_index = (curr_state_index + 1) % sprite_sheet_ptr->states.size();
	curr_frame = sprite_sheet_ptr->states[curr_state_index].first;
}

void Animation::setState(int new_state_index)
{
	if (new_state_index >= 0 && new_state_index < sprite_sheet_ptr->states.size()) {
		curr_state_index = new_state_index;
		curr_frame = sprite_sheet_ptr->states[curr_state_index].first;
	}
}

int SpriteSheet::getPlayerStateFromDirection(DIRECTION dir)
{
	switch (dir) {
		case DIRECTION::N:
			return (int)PLAYER_SPRITE_STATES::NORTH;
		case DIRECTION::NE:
			return (int)PLAYER_SPRITE_STATES::NORTH_EAST;
		case DIRECTION::E:
			return (int)PLAYER_SPRITE_STATES::EAST;
		case DIRECTION::SE:
			return (int)PLAYER_SPRITE_STATES::EAST;
		case DIRECTION::S:
			return (int)PLAYER_SPRITE_STATES::SOUTH;
		case DIRECTION::SW:
			return (int)PLAYER_SPRITE_STATES::EAST;
		case DIRECTION::W:
			return (int)PLAYER_SPRITE_STATES::EAST;
		case DIRECTION::NW:
			return (int)PLAYER_SPRITE_STATES::NORTH_EAST;
		default:
			return (int)PLAYER_SPRITE_STATES::EAST;
	}
}

bool SpriteSheet::getPlayerMirrored(DIRECTION dir)
{
	return (dir == DIRECTION::SW || dir == DIRECTION::W || dir == DIRECTION::NW);
}
