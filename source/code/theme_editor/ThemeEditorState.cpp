/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2026 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://blockattack.net
===========================================================================
*/

#include "ThemeEditorState.hpp"

#include <fmt/core.h>
#include <algorithm>

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "../sago/SagoMisc.hpp"
#include "../BlockGameSdl.hpp"

ThemeEditorState::ThemeEditorState() {
	current_border.name = "standard";
	current_border.border_sprite = "board_back_back";
	current_border.border_sprite_offset = {-60, -68};
	current_border.score_label_offset = {310, 80};
	current_border.time_label_offset = {310, 129};
	current_border.chain_label_offset = {310, 178};
	current_border.speed_label_offset = {310, 227};
	current_border.score_field_offset = {310, 102};
	current_border.time_field_offset = {310, 151};
	current_border.chain_field_offset = {310, 200};
	current_border.speed_field_offset = {310, 249};

	current_background.name = "standard";
	current_background.background_sprite = "background";
	current_background.background_sprite_16x9 = "background_sixteen_nine";
	current_background.background_scale = ImgScale::Stretch;
}

ThemeEditorState::~ThemeEditorState() {
	// Restore the original theme when editor exits
	globalData.theme = saved_theme;
}

bool ThemeEditorState::IsActive() {
	return isActive;
}

void ThemeEditorState::ProcessInput(const SDL_Event& event, bool& processed) {
	(void)event;
	(void)processed;
	// Handle any custom input processing here if needed
}

void ThemeEditorState::Init() {
	// Initialize the theme system
	ThemesInit();

	// Populate lists
	border_names = ThemesGetBorderNames();
	background_names = ThemesGetBackgroundNames();
	decoration_names = ThemesGetDecorationNames();

	// Save current theme and set up preview
	saved_theme = globalData.theme;
	preview_theme = ThemesGet(0);

	// Create preview game instance positioned in the right 40% of screen
	// Editor takes 60% of width, so preview starts at 60% + some padding
	int preview_x = static_cast<int>(globalData.xsize * 0.62f);
	preview_game = std::make_shared<BlockGameSdl>(preview_x, 100, &globalData.spriteHolder->GetDataHolder());
	preview_game->name = "Preview";
	preview_game->putSampleBlocks();

	// Initialize preview with current border/background
	preview_theme.border = current_border;
	preview_theme.background = current_background;
	globalData.theme = preview_theme;
}

void ThemeEditorState::Update() {
	// Update preview if needed
	if (preview_needs_update) {
		preview_needs_update = false;
		UpdatePreview();
	}
}

void ThemeEditorState::UpdatePreview() {
	// Update preview theme based on current tab
	switch (current_tab) {
		case 0: // Border Editor
			preview_theme.border = current_border;
			break;
		case 1: // Background Editor
			preview_theme.background = current_background;
			break;
		case 2: // Theme Composer
			// Combine selected components
			if (static_cast<size_t>(selected_border_index) < border_names.size()) {
				preview_theme.border = ThemesGetBorder(border_names[selected_border_index]);
			}
			if (static_cast<size_t>(selected_background_index) < background_names.size()) {
				preview_theme.background = ThemesGetBackground(background_names[selected_background_index]);
			}
			preview_theme.back_board = board_backgrounds[selected_board_background];
			break;
	}

	// Apply theme to global for preview rendering
	globalData.theme = preview_theme;
}

void ThemeEditorState::Draw(SDL_Renderer* target) {
	// Get actual physical renderer size and keep logicalResize in sync every frame
	int display_w = 1;
	int display_h = 1;
	SDL_GetCurrentRenderOutputSize(target, &display_w, &display_h);
	globalData.logicalResize.SetPhysicalSize(display_w, display_h);

	// Main window - take up left 60% of screen to leave room for preview
	const float editor_width = display_w * 0.5f;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(editor_width, (float)display_h), ImGuiCond_Always);

	ImGui::Begin("Theme Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	// Tab bar
	if (ImGui::BeginTabBar("EditorTabs")) {
		if (ImGui::BeginTabItem("Border Editor")) {
			current_tab = 0;
			DrawBorderEditor();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Background Editor")) {
			current_tab = 1;
			DrawBackgroundEditor();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Theme Composer")) {
			current_tab = 2;
			DrawThemeComposer();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::Separator();
	if (ImGui::Button("Exit Editor")) {
		isActive = false;
	}

	ImGui::End();

	// Draw the preview: convert physical position (ImGui space) to logical space for DoPaintJob
	if (preview_game) {
		const int preview_physical_x = static_cast<int>(editor_width) + 250;
		const int preview_physical_y = 150;
		int preview_logical_x = 0;
		int preview_logical_y = 0;
		globalData.logicalResize.PhysicalToLogical(preview_physical_x, preview_physical_y, preview_logical_x, preview_logical_y);
		preview_game->SetTopXY(preview_logical_x, preview_logical_y);
		preview_game->DoPaintJob();
	}
}

void ThemeEditorState::DrawBorderEditor() {
	ImGui::Text("Create and edit custom borders");
	ImGui::Separator();

	// Load existing border
	ImGui::Text("Load existing border:");
	if (ImGui::BeginCombo("##BorderLoad", border_names.empty() ? "No borders" : current_border.name.c_str())) {
		for (const auto& name : border_names) {
			bool is_selected = (current_border.name == name);
			if (ImGui::Selectable(name.c_str(), is_selected)) {
				LoadBorder(name);
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Border Settings:");

	// Border name
	ImGui::InputText("Border Name", border_name_buffer, sizeof(border_name_buffer));

	// Border sprite
	if (ImGui::InputText("Border Sprite", border_sprite_buffer, sizeof(border_sprite_buffer))) {
		current_border.border_sprite = border_sprite_buffer;
		preview_needs_update = true;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Examples: board_back_back, board_back_back_mirror");
	}

	// Validate sprite button
	ImGui::SameLine();
	if (ImGui::Button("Validate##border")) {
		if (ValidateSprite(border_sprite_buffer)) {
			ImGui::OpenPopup("Valid Sprite");
		} else {
			ImGui::OpenPopup("Invalid Sprite");
		}
	}

	if (ImGui::BeginPopup("Valid Sprite")) {
		ImGui::Text("Sprite exists!");
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopup("Invalid Sprite")) {
		ImGui::Text("Warning: Sprite does not exist!");
		ImGui::EndPopup();
	}

	// Border sprite offset
	if (ImGui::DragInt2("Border Sprite Offset", &border_offset_x)) {
		current_border.border_sprite_offset = {border_offset_x, border_offset_y};
		preview_needs_update = true;
	}

	// Label offsets
	ImGui::Separator();
	ImGui::Text("Label Positions:");
	if (ImGui::DragInt2("Score Label Offset", &score_offset_x)) {
		current_border.score_label_offset = {score_offset_x, score_offset_y};
		preview_needs_update = true;
	}
	if (ImGui::DragInt2("Time Label Offset", &time_offset_x)) {
		current_border.time_label_offset = {time_offset_x, time_offset_y};
		preview_needs_update = true;
	}
	if (ImGui::DragInt2("Chain Label Offset", &chain_offset_x)) {
		current_border.chain_label_offset = {chain_offset_x, chain_offset_y};
		preview_needs_update = true;
	}
	if (ImGui::DragInt2("Speed Label Offset", &speed_offset_x)) {
		current_border.speed_label_offset = {speed_offset_x, speed_offset_y};
		preview_needs_update = true;
	}

	// Field offsets
	ImGui::Separator();
	ImGui::Text("Field Positions:");
	if (ImGui::DragInt2("Score Field Offset", &score_field_x)) {
		current_border.score_field_offset = {score_field_x, score_field_y};
		preview_needs_update = true;
	}
	if (ImGui::DragInt2("Time Field Offset", &time_field_x)) {
		current_border.time_field_offset = {time_field_x, time_field_y};
		preview_needs_update = true;
	}
	if (ImGui::DragInt2("Chain Field Offset", &chain_field_x)) {
		current_border.chain_field_offset = {chain_field_x, chain_field_y};
		preview_needs_update = true;
	}
	if (ImGui::DragInt2("Speed Field Offset", &speed_field_x)) {
		current_border.speed_field_offset = {speed_field_x, speed_field_y};
		preview_needs_update = true;
	}

	ImGui::Separator();
	if (ImGui::Button("Save Border")) {
		SaveCurrentBorder();
	}
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Saved to: borders/<name>.json");
}

void ThemeEditorState::DrawBackgroundEditor() {
	ImGui::Text("Create and edit custom backgrounds");
	ImGui::Separator();

	// Load existing background
	ImGui::Text("Load existing background:");
	if (ImGui::BeginCombo("##BackgroundLoad", background_names.empty() ? "No backgrounds" : current_background.name.c_str())) {
		for (const auto& name : background_names) {
			bool is_selected = (current_background.name == name);
			if (ImGui::Selectable(name.c_str(), is_selected)) {
				LoadBackground(name);
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Background Settings:");

	// Background name
	ImGui::InputText("Background Name", background_name_buffer, sizeof(background_name_buffer));

	// Background sprite
	if (ImGui::InputText("Background Sprite", background_sprite_buffer, sizeof(background_sprite_buffer))) {
		current_background.background_sprite = background_sprite_buffer;
		preview_needs_update = true;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Examples: background, background_autumn, custom_background_<filename>");
	}

	// Validate sprite button
	ImGui::SameLine();
	if (ImGui::Button("Validate##background")) {
		if (ValidateSprite(background_sprite_buffer)) {
			ImGui::OpenPopup("Valid BG Sprite");
		} else {
			ImGui::OpenPopup("Invalid BG Sprite");
		}
	}

	if (ImGui::BeginPopup("Valid BG Sprite")) {
		ImGui::Text("Sprite exists!");
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopup("Invalid BG Sprite")) {
		ImGui::Text("Warning: Sprite does not exist!");
		ImGui::EndPopup();
	}

	// 16:9 background sprite (optional)
	if (ImGui::InputText("Background Sprite 16:9 (optional)", background_sprite_16x9_buffer, sizeof(background_sprite_16x9_buffer))) {
		current_background.background_sprite_16x9 = background_sprite_16x9_buffer;
		preview_needs_update = true;
	}

	// Scaling mode
	ImGui::Separator();
	ImGui::Text("Scaling Mode:");
	const char* scale_modes[] = { "Stretch", "Tile", "Resize", "Cut" };
	if (ImGui::Combo("##ScaleMode", &background_scale_mode, scale_modes, 4)) {
		switch (background_scale_mode) {
			case 0: current_background.background_scale = ImgScale::Stretch; break;
			case 1: current_background.background_scale = ImgScale::Tile; break;
			case 2: current_background.background_scale = ImgScale::Resize; break;
			case 3: current_background.background_scale = ImgScale::Cut; break;
		}
		preview_needs_update = true;
	}

	// Tile animation speed
	ImGui::Separator();
	ImGui::Text("Tile Animation (0 = disabled):");
	if (ImGui::DragInt("Move Speed X (ms)", &tile_move_speed_x, 1.0f, 0, 10000)) {
		current_background.tileMoveSpeedX = tile_move_speed_x;
		preview_needs_update = true;
	}
	if (ImGui::DragInt("Move Speed Y (ms)", &tile_move_speed_y, 1.0f, 0, 10000)) {
		current_background.tileMoveSpeedY = tile_move_speed_y;
		preview_needs_update = true;
	}

	ImGui::Separator();
	if (ImGui::Button("Save Background")) {
		SaveCurrentBackground();
	}
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Saved to: backgrounds/<name>.json");
}

void ThemeEditorState::DrawThemeComposer() {
	ImGui::Text("Combine borders and backgrounds into a complete theme");
	ImGui::Separator();

	// Theme name
	ImGui::InputText("Theme Name", theme_name_buffer, sizeof(theme_name_buffer));

	// Border selection
	ImGui::Separator();
	ImGui::Text("Select Border:");
	if (ImGui::BeginCombo("##BorderSelect", static_cast<size_t>(selected_border_index) < border_names.size() ? border_names[selected_border_index].c_str() : "")) {
		for (size_t i = 0; i < border_names.size(); i++) {
			bool is_selected = (static_cast<size_t>(selected_border_index) == i);
			if (ImGui::Selectable(border_names[i].c_str(), is_selected)) {
				selected_border_index = i;
				preview_needs_update = true;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Background selection
	ImGui::Text("Select Background:");
	if (ImGui::BeginCombo("##BackgroundSelect", static_cast<size_t>(selected_background_index) < background_names.size() ? background_names[selected_background_index].c_str() : "")) {
		for (size_t i = 0; i < background_names.size(); i++) {
			bool is_selected = (static_cast<size_t>(selected_background_index) == i);
			if (ImGui::Selectable(background_names[i].c_str(), is_selected)) {
				selected_background_index = i;
				preview_needs_update = true;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Decoration selection
	ImGui::Text("Select Decoration:");
	if (ImGui::BeginCombo("##DecorationSelect", static_cast<size_t>(selected_decoration_index) < decoration_names.size() ? decoration_names[selected_decoration_index].c_str() : "")) {
		for (size_t i = 0; i < decoration_names.size(); i++) {
			bool is_selected = (static_cast<size_t>(selected_decoration_index) == i);
			if (ImGui::Selectable(decoration_names[i].c_str(), is_selected)) {
				selected_decoration_index = i;
				preview_needs_update = true;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Board background selection
	ImGui::Text("Select Board Background:");
	if (ImGui::BeginCombo("##BoardBGSelect", board_backgrounds[selected_board_background].c_str())) {
		for (size_t i = 0; i < board_backgrounds.size(); i++) {
			bool is_selected = (static_cast<size_t>(selected_board_background) == i);
			if (ImGui::Selectable(board_backgrounds[i].c_str(), is_selected)) {
				selected_board_background = i;
				preview_needs_update = true;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
	if (ImGui::Button("Save Theme")) {
		SaveCurrentTheme();
	}
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Saved to: custom_themes.json");
}

void ThemeEditorState::DrawPreview(SDL_Renderer* target) {
	(void)target;
	// TODO: Implement preview rendering using BlockGameSdl
}

void ThemeEditorState::SaveCurrentBorder() {
	// Update current_border from UI fields
	std::string name = border_name_buffer;
	if (name.empty()) {
		std::cerr << "Error: Border name cannot be empty\n";
		return;
	}

	current_border.name = name;
	current_border.border_sprite = border_sprite_buffer;
	current_border.border_sprite_offset = {border_offset_x, border_offset_y};
	current_border.score_label_offset = {score_offset_x, score_offset_y};
	current_border.time_label_offset = {time_offset_x, time_offset_y};
	current_border.chain_label_offset = {chain_offset_x, chain_offset_y};
	current_border.speed_label_offset = {speed_offset_x, speed_offset_y};
	current_border.score_field_offset = {score_field_x, score_field_y};
	current_border.time_field_offset = {time_field_x, time_field_y};
	current_border.chain_field_offset = {chain_field_x, chain_field_y};
	current_border.speed_field_offset = {speed_field_x, speed_field_y};

	// Validate sprite
	if (!ValidateSprite(current_border.border_sprite)) {
		std::cerr << "Warning: Saving border with non-existent sprite: " << current_border.border_sprite << "\n";
	}

	// Save to file
	ThemesSaveBorderData(name, current_border);
	std::cout << "Saved border: " << name << "\n";

	// Refresh border list
	border_names = ThemesGetBorderNames();
}

void ThemeEditorState::SaveCurrentBackground() {
	// Update current_background from UI fields
	std::string name = background_name_buffer;
	if (name.empty()) {
		std::cerr << "Error: Background name cannot be empty\n";
		return;
	}

	current_background.name = name;
	current_background.background_sprite = background_sprite_buffer;
	current_background.background_sprite_16x9 = background_sprite_16x9_buffer;

	// Convert scale mode
	switch (background_scale_mode) {
		case 0: current_background.background_scale = ImgScale::Stretch; break;
		case 1: current_background.background_scale = ImgScale::Tile; break;
		case 2: current_background.background_scale = ImgScale::Resize; break;
		case 3: current_background.background_scale = ImgScale::Cut; break;
	}

	current_background.tileMoveSpeedX = tile_move_speed_x;
	current_background.tileMoveSpeedY = tile_move_speed_y;

	// Validate sprite
	if (!ValidateSprite(current_background.background_sprite)) {
		std::cerr << "Warning: Saving background with non-existent sprite: " << current_background.background_sprite << "\n";
	}

	// Save to file
	ThemesSaveBackgroundData(name, current_background);
	std::cout << "Saved background: " << name << "\n";

	// Refresh background list
	background_names = ThemesGetBackgroundNames();
}

void ThemeEditorState::SaveCurrentTheme() {
	std::string name = theme_name_buffer;
	if (name.empty()) {
		std::cerr << "Error: Theme name cannot be empty\n";
		return;
	}

	Theme theme;
	theme.theme_name = name;

	// Set components
	if (static_cast<size_t>(selected_border_index) < border_names.size()) {
		theme.border = ThemesGetBorder(border_names[selected_border_index]);
	}
	if (static_cast<size_t>(selected_background_index) < background_names.size()) {
		theme.background = ThemesGetBackground(background_names[selected_background_index]);
	}
	theme.back_board = board_backgrounds[selected_board_background];

	// Add or replace the theme
	ThemesAddOrReplace(theme);

	// Save to custom themes file
	ThemesSaveCustomSlots();

	std::cout << "Saved theme: " << name << "\n";
}

void ThemeEditorState::LoadBorder(const std::string& name) {
	current_border = ThemesGetBorder(name);

	// Update UI fields
	strncpy(border_name_buffer, current_border.name.c_str(), sizeof(border_name_buffer) - 1);
	strncpy(border_sprite_buffer, current_border.border_sprite.c_str(), sizeof(border_sprite_buffer) - 1);
	border_offset_x = current_border.border_sprite_offset.first;
	border_offset_y = current_border.border_sprite_offset.second;
	score_offset_x = current_border.score_label_offset.first;
	score_offset_y = current_border.score_label_offset.second;
	time_offset_x = current_border.time_label_offset.first;
	time_offset_y = current_border.time_label_offset.second;
	chain_offset_x = current_border.chain_label_offset.first;
	chain_offset_y = current_border.chain_label_offset.second;
	speed_offset_x = current_border.speed_label_offset.first;
	speed_offset_y = current_border.speed_label_offset.second;
	score_field_x = current_border.score_field_offset.first;
	score_field_y = current_border.score_field_offset.second;
	time_field_x = current_border.time_field_offset.first;
	time_field_y = current_border.time_field_offset.second;
	chain_field_x = current_border.chain_field_offset.first;
	chain_field_y = current_border.chain_field_offset.second;
	speed_field_x = current_border.speed_field_offset.first;
	speed_field_y = current_border.speed_field_offset.second;
	preview_needs_update = true;
}

void ThemeEditorState::LoadBackground(const std::string& name) {
	current_background = ThemesGetBackground(name);

	// Update UI fields
	strncpy(background_name_buffer, current_background.name.c_str(), sizeof(background_name_buffer) - 1);
	strncpy(background_sprite_buffer, current_background.background_sprite.c_str(), sizeof(background_sprite_buffer) - 1);
	strncpy(background_sprite_16x9_buffer, current_background.background_sprite_16x9.c_str(), sizeof(background_sprite_16x9_buffer) - 1);

	// Convert scale mode
	switch (current_background.background_scale) {
		case ImgScale::Stretch: background_scale_mode = 0; break;
		case ImgScale::Tile: background_scale_mode = 1; break;
		case ImgScale::Resize: background_scale_mode = 2; break;
		case ImgScale::Cut: background_scale_mode = 3; break;
	}

	tile_move_speed_x = current_background.tileMoveSpeedX;
	tile_move_speed_y = current_background.tileMoveSpeedY;
	preview_needs_update = true;
}

bool ThemeEditorState::ValidateSprite(const std::string& sprite_name) {
	return ThemesValidateSpriteName(sprite_name);
}

std::vector<std::string> ThemeEditorState::GetAvailableSprites() {
	// TODO: Implement getting available sprite names from globalData.spriteHolder
	return std::vector<std::string>();
}
