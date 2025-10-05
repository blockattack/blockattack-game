/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2025 Poul Sander

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
#include "../BlockGameSdl.hpp"
#include <string>
#include <vector>
#include <array>

#define selection_clear 10
#define selection_move_up 11
#define selection_move_down 12
#define selection_move_left 13
#define selection_move_right 14

struct PuzzleState {
	std::array<std::array<std::array<int, 12>, 6>, 50> puzzle_data;
	std::vector<int> moves_allowed;
	int puzzle_count;
	int current_puzzle;
};

class PuzzleEditorState : public sago::GameStateInterface {
public:
	PuzzleEditorState();
	PuzzleEditorState(const PuzzleEditorState& orig) = delete;
	virtual ~PuzzleEditorState() = default;

	void Init();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

private:
	void SelectFile(const std::string& file);

	void BrickClicked(int x, int y);
	void SaveCurrentState();
	void Undo();
	void Redo();
	bool CanUndo() const;
	bool CanRedo() const;

	int selected_action = 0;

	bool isActive = true;
	bool read_only = true;
	int selected_puzzle = -1;
	std::string selected_file;
	std::vector<std::string> puzzle_files;
	sago::SagoLogicalResize window_resize;

	std::map<std::string, std::vector<PuzzleState> > undo_file_stack;
	std::map<std::string, std::vector<PuzzleState> > redo_file_stack;
	std::vector<PuzzleState> undo_stack;
	std::vector<PuzzleState> redo_stack;
	static const size_t MAX_UNDO_STATES = 50;
};
