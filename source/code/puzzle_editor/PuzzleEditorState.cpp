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

#include <unistd.h>
#include <fmt/core.h>

#include "imgui.h"
#include "imgui_internal.h"
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
	read_only = false;
	if (file == "puzzle.levels") {
		read_only = true;
	}
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


static void ImGuiWritePartOfTexture(SDL_Texture* texture, int topx, int topy, int tex_w, int tex_h, int w, int h) {
	float sprite_w = tex_w;
	float sprite_h = tex_h;
	float topxf = topx;
	float topyf = topy;
	ImVec2 uv0 = ImVec2(topxf / tex_w, topyf / tex_h);
	ImVec2 uv1 = ImVec2((topxf + sprite_w) / tex_w, (topyf + sprite_h) / tex_h);
	ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)w, (float)h), uv0, uv1);
}


static void DrawBrick(int brick, int x, int y, int width, int height) {
	if (brick < 0 || brick > 6) {
		return;
	}

	sago::SagoSprite& sprite = globalData.bricks[brick];
	ImGui::SetCursorScreenPos({ static_cast<float>(x), static_cast<float>(y) });
	ImGuiWritePartOfTexture(sprite.GetTextureHandler().get(), sprite.GetImageCord().x, sprite.GetImageCord().y, sprite.GetWidth(), sprite.GetHeight(), width, height);
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


	int brick_size = 50;
	window_resize.LogicalToPhysical(&brick_size, nullptr);
	brick_size -= window_resize.GetLeftMargin();
	for (int i=0; i < 6; ++i) {
		for (int j=0; j < 12; ++j) {
			ImVec2 p1(i*50.0f, (11-j)*50.0f);
			LogicalToPhysical(window_resize, p1);
			p1.x += xoffset;
			p1.y += yoffset;
			int brick = PuzzleGetBrick(this->selected_puzzle, i, j);
			DrawBrick(brick, static_cast<int>(p1.x), static_cast<int>(p1.y), brick_size, brick_size);
		}
	}

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
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMouseReleased(0) && io.MouseDownDurationPrev[0] < 0.3f && !ImGui::IsMouseDragPastThreshold(0)) {
		ImVec2 pos = io.MousePos;
		pos.x -= xoffset;
		pos.y -= yoffset;
		int logical_pos_x = 0;
		int logical_pos_y = 0;
		window_resize.PhysicalToLogical(pos.x, pos.y, logical_pos_x, logical_pos_y);
		//std::cout << "Clicked at: " << pos.x << "," << pos.y << "\n";
		//std::cout << "Logical Pos: " << logical_pos_x << "," << logical_pos_y << "\n";
		int board_x = logical_pos_x / 50;
		int board_y = logical_pos_y / 50;
		if (board_x >=0 && board_x < 6 && board_y >= 0 && board_y < 12) {
			std::cout << "Board Pos: " << board_x << "," << board_y << "\n";
		}

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

	ImGui::Begin("Palette");
	for (size_t i = 0; i < 7; ++i) {
		if (ImGui::Selectable(fmt::format("##{}", i).c_str(), this->selected_action == i, 0, ImVec2(0,50))) {
			this->selected_action = i;
		}
		ImGui::SameLine();
		sago::SagoSprite& sprite = globalData.bricks[i];
		ImGuiWritePartOfTexture(sprite.GetTextureHandler().get(), sprite.GetImageCord().x, sprite.GetImageCord().y, sprite.GetWidth(), sprite.GetHeight(), 50, 50);
	}
	if (ImGui::Selectable("Clear", this->selected_action == selection_clear)) {
		this->selected_action = selection_clear;
	}
	if (ImGui::Selectable("Move Up", this->selected_action == selection_move_up)) {
		this->selected_action = selection_move_up;
	}
	if (ImGui::Selectable("Move down", this->selected_action == selection_move_down)) {
		this->selected_action = selection_move_down;
	}
	if (ImGui::Selectable("Move left", this->selected_action == selection_move_left)) {
		this->selected_action = selection_move_left;
	}
	if (ImGui::Selectable("Move right", this->selected_action == selection_move_right)) {
		this->selected_action = selection_move_right;
	}
	ImGui::Separator();
	ImGui::LabelText("moves allowed", "%i", PuzzleNumberOfMovesAllowed(this->selected_puzzle));
	ImGui::Separator();
	if (read_only) {
		ImGui::LabelText("Read Only", "");
		ImGui::Separator();
	}
	ImGui::End();
}

void PuzzleEditorState::Update() {
}

