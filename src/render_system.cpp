// internal
#include "render_system.hpp"
#include <SDL.h>
#include <iostream>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
	const mat3& projection)
{
	Position& position = registry.positions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(position.position);
	transform.rotate(position.angle);
	transform.scale(position.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED || 
		render_request.used_effect == EFFECT_ASSET_ID::RESOURCE_BAR || 
		render_request.used_effect == EFFECT_ASSET_ID::ANIMATED ||
		render_request.used_effect == EFFECT_ASSET_ID::REPEAT)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		if (render_request.used_effect == EFFECT_ASSET_ID::RESOURCE_BAR) {
			float fraction = 0.0;
			float logoRatio = 0.0;
			float barRatio = 1.0;
			if (render_request.used_texture == TEXTURE_ASSET_ID::PLAYER_HEALTH_BAR || render_request.used_texture == TEXTURE_ASSET_ID::ENEMY_HEALTH_BAR) {
				assert(registry.healthBars.has(entity));
				Follower& follower = registry.followers.get(entity);
				assert(registry.resources.has(follower.owner));
				Resources& resources = registry.resources.get(follower.owner);
				fraction = resources.currentHealth / resources.maxHealth;
				logoRatio = resources.logoRatio;
				barRatio = resources.barRatio;
			}
			else if (render_request.used_texture == TEXTURE_ASSET_ID::PLAYER_MANA_BAR || render_request.used_texture == TEXTURE_ASSET_ID::ENEMY_MANA_BAR) {
				assert(registry.manaBars.has(entity));
				Follower& follower = registry.followers.get(entity);
				assert(registry.resources.has(follower.owner));
				Resources& resources = registry.resources.get(follower.owner);
				fraction = resources.currentMana / resources.maxMana;
				logoRatio = resources.logoRatio;
				barRatio = resources.barRatio;
			}
			glUniform1f(glGetUniformLocation(program, "fraction"), fraction);
			glUniform1f(glGetUniformLocation(program, "logoRatio"), logoRatio);
			glUniform1f(glGetUniformLocation(program, "barRatio"), barRatio);
			gl_has_errors();
		}
		else if (render_request.used_effect == EFFECT_ASSET_ID::ANIMATED) {
			assert(registry.animations.has(entity));
			Animation& animation = registry.animations.get(entity);
			assert(animation.sprite_sheet_ptr != nullptr);
			glUniform1f(glGetUniformLocation(program, "time"), (float)(glfwGetTime() * 10.0f));
			glUniform1i(glGetUniformLocation(program, "frame_col"), animation.getColumn());
			glUniform1i(glGetUniformLocation(program, "frame_row"), animation.getRow());
			glUniform1f(glGetUniformLocation(program, "frame_width"), animation.sprite_sheet_ptr->getFrameSizeInTexcoords().x);
			glUniform1f(glGetUniformLocation(program, "frame_height"), animation.sprite_sheet_ptr->getFrameSizeInTexcoords().y);
			glUniform1i(glGetUniformLocation(program, "rainbow_enabled"), animation.rainbow_enabled);
			gl_has_errors();
		}
		else if (render_request.used_effect == EFFECT_ASSET_ID::REPEAT) {
			float x_scale = 1;
			float y_scale = 1;
			if (registry.terrain.has(entity)) {
				switch (registry.directions.get(entity).direction) {
					case DIRECTION::N: // north
					case DIRECTION::S: // south
						x_scale = position.scale.x / 100;
						break;
					case DIRECTION::E: // side
						y_scale = position.scale.y / 100;
						break;
					case DIRECTION::W: // generic
						x_scale = position.scale.x / 100;
						y_scale = position.scale.y / 100;
					default:				
						break;
				}
			}
			else if (registry.floors.has(entity)) {
				x_scale = position.scale.x / 100;
				y_scale = position.scale.y / 100;
			}
			glUniform1f(glGetUniformLocation(program, "x_scale"), x_scale);
			glUniform1f(glGetUniformLocation(program, "y_scale"), y_scale);
		}
	}
	// This is kind of useless now
	else if (render_request.used_effect == EFFECT_ASSET_ID::PLAYER || render_request.used_effect == EFFECT_ASSET_ID::EXIT_DOOR)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

		if (render_request.used_effect == EFFECT_ASSET_ID::PLAYER) {

			float time = (float) glfwGetTime();
			vec3 initial_color = vec3(0.3f, 0.0f, 0.0f);
			vec3 final_color = vec3(0.8f, 0.0f, 0.0f);
			vec3 color_change = initial_color + (final_color - initial_color) * sin(time);

			GLuint change_uloc = glGetUniformLocation(program, "change");
			glUniform3f(change_uloc, color_change.x, color_change.y, color_change.z);
			gl_has_errors();
		}
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::SHADOW) {
		if (!registry.shadows.get(entity).active) return;
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the lighting texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::DARKEN]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
	// indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint darken_program = effects[(GLuint)EFFECT_ASSET_ID::DARKEN];

	// Pass light radius to the post-processing shader
	glUniform1f(glGetUniformLocation(darken_program, "light_radius"), light_radius);

	// Set clock
	GLuint window_size_uloc = glGetUniformLocation(darken_program, "window_size");
	GLuint dead_timer_uloc = glGetUniformLocation(darken_program, "screen_darken_factor");
	GLuint radius_uloc = glGetUniformLocation(darken_program, "radius");
	GLuint apply_spotlight_bool = glGetUniformLocation(darken_program, "apply_spotlight");
	
	ScreenState& screen = registry.screenStates.get(screen_state_entity);

	glUniform2f(window_size_uloc, window_width_px, window_height_px);
	glUniform1f(radius_uloc, screen.spotlight_radius);
	glUniform1f(apply_spotlight_bool, screen.apply_spotlight);
	glUniform1f(dead_timer_uloc, screen.screen_darken_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(darken_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
	// no offset from the bound index buffer
	gl_has_errors();
}

void RenderSystem::drawArsenal(Entity entity, const mat3& projection){
	Position& position = registry.positions.get(entity);
	Transform transform;
	transform.translate(position.position);
	transform.rotate(position.angle);
	transform.scale(position.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::ANIMATED];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
	gl_has_errors();
	assert(in_texcoord_loc >= 0);

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(TexturedVertex), (void*)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(
		in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
		(void*)sizeof(
			vec3)); // note the stride to skip the preceeding vertex position

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	gl_has_errors();

	assert(registry.renderRequests.has(entity));
	GLuint texture_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

	glBindTexture(GL_TEXTURE_2D, texture_id);
	gl_has_errors();

	assert(registry.animations.has(entity));
	Animation& animation = registry.animations.get(entity);
	assert(animation.sprite_sheet_ptr != nullptr);
	glUniform1f(glGetUniformLocation(program, "time"), (float)(glfwGetTime() * 10.0f));
	glUniform1i(glGetUniformLocation(program, "frame_col"), animation.getColumn());
	glUniform1i(glGetUniformLocation(program, "frame_row"), animation.getRow());
	glUniform1f(glGetUniformLocation(program, "frame_width"), animation.sprite_sheet_ptr->getFrameSizeInTexcoords().x);
	glUniform1f(glGetUniformLocation(program, "frame_height"), animation.sprite_sheet_ptr->getFrameSizeInTexcoords().y);
	glUniform1i(glGetUniformLocation(program, "rainbow_enabled"), animation.rainbow_enabled);
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
	// and alpha blending, one would have to sort
	// sprites back to front
	gl_has_errors();

	// get to players position
	assert(registry.players.size() >= 1);
	Entity entity = registry.players.entities[0];
	Position& player_pos = registry.positions.get(entity);

	// center the camera on the player
	Camera camera;
	camera.centerAt(player_pos.position);

	// Handle drawing floors first
	for (Entity entity : registry.floors.entities) {
		drawTexturedMesh(entity, camera.projectionMat);
	}

	// Handle all shadows next
	for (Entity entity : registry.shadows.entities) {
		drawTexturedMesh(entity, camera.projectionMat);
	}

	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.positions.has(entity) || registry.texts.has(entity) || 
			registry.shadows.has(entity) || registry.floors.has(entity) ||
			registry.projectileSelectDisplays.has(entity) || registry.healthBars.has(entity) ||
			registry.manaBars.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, camera.projectionMat);
	}

	// Truely render to the screen
	drawToScreen();

	// We do this after post processing the lighting effect
	for (Entity entity : registry.healthBars.entities) {
		drawTexturedMesh(entity, camera.projectionMat);
	}
	
	for (Entity entity : registry.manaBars.entities) {
		drawTexturedMesh(entity, camera.projectionMat);
	}

	for (Entity entity : registry.texts.entities)
	{
		drawText(entity);
	}

	for (Entity entity : registry.projectileSelectDisplays.entities) {
		drawArsenal(entity, camera.projectionMat);
	}
  
	// Render ImGui to screen
	drawImGui();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

void RenderSystem::drawText(Entity entity) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Position& position = registry.positions.get(entity);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	Text& text_component = registry.texts.get(entity);
	float scale = position.scale.x;
	std::string text = text_component.text;
	vec3 color = text_component.color;
	GLint vertex_loc = glGetAttribLocation(program, "vertex");
	glEnableVertexAttribArray(vertex_loc);
	glVertexAttribPointer(vertex_loc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	// iterate through all characters
	std::string::const_iterator c;
	float x = position.position.x;
	float y = position.position.y;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	mat4 text_projection = ortho(0.0f, static_cast<float>(window_width_px), 0.0f, static_cast<float>(window_height_px));
	glUniformMatrix4fv(glGetUniformLocation(currProgram, "projection"), 1, GL_FALSE, (float*)&text_projection);
	gl_has_errors();
	return;
}

void RenderSystem::animation_step(float elapsed_ms)
{
	elapsed_time += elapsed_ms;
	if (elapsed_time > ANIMATION_SPEED) {
		elapsed_time = 0.f;
		for (uint i = 0; i < registry.animations.size(); i++) {
 			Animation& animation = registry.animations.components[i];
			if (animation.is_animating) {
				animation.advanceFrame();
			}
		}
	}
}
