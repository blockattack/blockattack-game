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

#pragma once

#include "../global.hpp"
#include "../sago/GameStateInterface.hpp"
#include "../themes.hpp"
#include <string>
#include <vector>
#include <memory>

class BlockGameSdl;

class ThemeEditorState : public sago::GameStateInterface {
public:
	ThemeEditorState();
	ThemeEditorState(const ThemeEditorState& orig) = delete;
	virtual ~ThemeEditorState();

	void Init();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

private:
	void DrawBorderEditor();
	void DrawBackgroundEditor();
	void DrawThemeComposer();
	void DrawPreview(SDL_Renderer* target);

	void SaveCurrentBorder();
	void SaveCurrentBackground();
	void SaveCurrentTheme();

	void LoadBorder(const std::string& name);
	void LoadBackground(const std::string& name);

	bool ValidateSprite(const std::string& sprite_name);
	std::vector<std::string> GetAvailableSprites();

	bool isActive = true;
	int current_tab = 0; // 0=Border, 1=Background, 2=Theme

	// Border editor state
	ThemeBorderData current_border;
	char border_name_buffer[256] = "";
	char border_sprite_buffer[256] = "board_back_back";
	int border_offset_x = -60;
	int border_offset_y = -68;
	int score_offset_x = 310;
	int score_offset_y = 80;
	int time_offset_x = 310;
	int time_offset_y = 129;
	int chain_offset_x = 310;
	int chain_offset_y = 178;
	int speed_offset_x = 310;
	int speed_offset_y = 227;

	// Background editor state
	BackGroundData current_background;
	char background_name_buffer[256] = "";
	char background_sprite_buffer[256] = "background";
	char background_sprite_16x9_buffer[256] = "background_sixteen_nine";
	int background_scale_mode = 0; // 0=Stretch, 1=Tile, 2=Resize, 3=Cut
	int tile_move_speed_x = 0;
	int tile_move_speed_y = 0;

	// Theme composer state
	char theme_name_buffer[256] = "custom_theme";
	int selected_border_index = 0;
	int selected_background_index = 0;
	int selected_decoration_index = 0;
	int selected_board_background = 0; // 0=back_board, 1=back_board_sample_snow, 2=trans_cover
	std::vector<std::string> border_names;
	std::vector<std::string> background_names;
	std::vector<std::string> decoration_names;
	const std::vector<std::string> board_backgrounds = {"back_board", "back_board_sample_snow", "trans_cover"};

	// Preview
	std::shared_ptr<BlockGameSdl> preview_game;
	Theme preview_theme;
	Theme saved_theme;  // Backup of the global theme
	bool preview_needs_update = true;
	void UpdatePreview();
};
