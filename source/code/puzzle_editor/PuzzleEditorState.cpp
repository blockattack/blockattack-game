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
#include "../sago/SagoMisc.hpp"
#include "../puzzlehandler.hpp"


PuzzleEditorState::PuzzleEditorState() {
	this->window_resize = sago::SagoLogicalResize(BOARD_WIDTH+5, BOARD_HEIGHT+5);
}

bool PuzzleEditorState::IsActive() {
	return isActive;
}

void PuzzleEditorState::ProcessInput(const SDL_Event& event, bool& processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void PuzzleEditorState::Init() {
	this->selected_file = "";
	this->puzzle_files = sago::GetFileList("puzzles");
}

void PuzzleEditorState::SelectFile(const std::string& file) {
	this->selected_file = file;
	PuzzleSetName(file);
	LoadPuzzleStages();
}

static void LogicalToPhysical(const sago::SagoLogicalResize& resize, ImVec2& inout) {
	int inx = inout.x;
	int iny = inout.y;
	int outx = inout.x;
	int outy = inout.y;
	resize.LogicalToPhysical(inx, iny, outx, outy);
	inout.x = outx;
	inout.y = outy;
}

void PuzzleEditorState::Draw(SDL_Renderer* target) {
	DrawBackground(target);

	ImGui::Begin("Board area");
	ImGui::BeginChild("Test");
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetContentRegionAvail();
	int xoffset = p.x;
	int yoffset = p.y;
	int height = BOARD_HEIGHT;
	int width = BOARD_WIDTH;
	window_resize.SetPhysicalSize(size.x, size.y);


	for (int i=0; i <= 6;++i) {
		ImVec2 p1(i*50.0f, 0);
		ImVec2 p2(i*50.0f, height);
		LogicalToPhysical(window_resize, p1);
		LogicalToPhysical(window_resize, p2);
		p1.x += xoffset;
		p1.y += yoffset;
		p2.x += xoffset;
		p2.y += yoffset;
		ImGui::GetWindowDrawList()->AddLine(p1, p2, IM_COL32(255, 0, 0, 100));
	}
	for (int i=0; i <= 12;++i) {
		ImVec2 p1(0,i*50.0f);
		ImVec2 p2(width, i*50.0f);
		LogicalToPhysical(window_resize, p1);
		LogicalToPhysical(window_resize, p2);
		p1.x += xoffset;
		p1.y += yoffset;
		p2.x += xoffset;
		p2.y += yoffset;
		ImGui::GetWindowDrawList()->AddLine(p1, p2, IM_COL32(255, 0, 0, 100));
	}
	ImGui::EndChild();
	ImGui::End();

	ImGui::Begin("File list", nullptr, ImGuiWindowFlags_NoCollapse);
	for (const auto& file : this->puzzle_files) {
		if (ImGui::Selectable(file.c_str(), this->selected_file == file)) {
			SelectFile(file);
		}
	}


	ImGui::End();
	ImGui::Begin("Puzzles in file");
	int puzzle_count = PuzzleGetNumberOfPuzzles();
	for (int i = 0; i < puzzle_count; ++i) {
		if (ImGui::Selectable(std::to_string(i).c_str(), this->selected_puzzle == i)) {
			this->selected_puzzle = i;
		}
	}
	ImGui::End();
}

void PuzzleEditorState::Update() {
}

