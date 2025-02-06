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

#include "PuzzleEditorState.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"



bool PuzzleEditorState::IsActive() {
	return isActive;
}

void PuzzleEditorState::ProcessInput(const SDL_Event& event, bool& processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void PuzzleEditorState::Draw(SDL_Renderer* target) {
	ImGui::Begin("File list", nullptr, ImGuiWindowFlags_NoCollapse);
	if (ImGui::Selectable("File 1", this->selected_file == "File 1")) {
		 this->selected_file = "File 1";
	}
	if (ImGui::Selectable("File 2",  this->selected_file == "File 2")) {
		 this->selected_file = "File 2";
	}

	ImGui::End();
}

void PuzzleEditorState::Update() {
}

