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


class PuzzleEditorState : public sago::GameStateInterface {
public:
	PuzzleEditorState() = default;
	PuzzleEditorState(const PuzzleEditorState& orig) = delete;
	virtual ~PuzzleEditorState() = default;

	void Init();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

private:
	void SelectFile(const std::string& file);

	bool isActive = true;
	int selected_puzzle = -1;
	std::string selected_file;
	std::vector<std::string> puzzle_files;
};
