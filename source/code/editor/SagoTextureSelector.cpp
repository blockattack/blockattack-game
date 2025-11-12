/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2024 Poul Sander

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
https://github.com/sago007/saland
===========================================================================
*/

#include "SagoTextureSelector.hpp"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL_image.h>
#include "../../sago/SagoMisc.hpp"
#include "../global.hpp"
#include "../common.h"

const char* const editor_texture_grid_size = "editor_texture_grid_size";


class ChangeSDLColor
{
public:
	ChangeSDLColor(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) : renderer(renderer) {
		SDL_GetRenderDrawColor(renderer, &oldr, &oldg, &oldb, &olda);
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
	}

	ChangeSDLColor(SDL_Renderer* renderer) : renderer(renderer) {
		SDL_GetRenderDrawColor(renderer, &oldr, &oldg, &oldb, &olda);
	}

	void setRed() {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	}

	void setYellow() {
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
	}

	void reset() {
		SDL_SetRenderDrawColor(renderer, oldr, oldg, oldb, olda);
	}

	~ChangeSDLColor() {
		SDL_SetRenderDrawColor(renderer, oldr, oldg, oldb, olda);
	}

private:
	SDL_Renderer* renderer;
	Uint8 oldr, oldg, oldb, olda;
};


static void addLinesToCanvas(SDL_Renderer* renderer, SDL_Texture* texture, int xstep = 32, int ystep = 32, int xoffset = 0, int yoffset = 0) {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

	if (xstep > 0) {
		for (int i = 0; i < width+1; i += xstep) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(i+xoffset, yoffset), ImVec2(i+xoffset, height+yoffset), IM_COL32(r, g, b, a));
		}
	}
	if (ystep > 0) {
		for (int i = 0; i < height+1; i += ystep) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(xoffset, i+yoffset), ImVec2(width+xoffset, i+yoffset), IM_COL32(r, g, b, a));
		}
	}
}

static void addRectableToCanvas(SDL_Renderer* renderer, int topx = 0, int topy = 0, int height = 100, int width = 100, int xoffset = 0, int yoffset = 0) {
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	topx += xoffset;
	topy += yoffset;
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx, topy), ImVec2(topx+width, topy), IM_COL32(r, g, b, a));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx+width, topy), ImVec2(topx+width, topy+height), IM_COL32(r, g, b, a));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx+width, topy+height), ImVec2(topx, topy+height), IM_COL32(r, g, b, a));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(topx, topy+height), ImVec2(topx, topy), IM_COL32(r, g, b, a));
}

static void addFolderToList(const std::string& folder, std::vector<std::string>& list, const std::string& filter = "") {
	std::vector<std::string> textures = sago::GetFileList(folder.c_str());
	for (const auto& texture : textures) {
		std::cout << "Texture: " << texture << "\n";
		if (texture.find(".png") == std::string::npos) {
			continue;
		}
		if (filter.empty() || texture.find(filter) != std::string::npos) {
			list.push_back(texture);
		}
	}
}

static std::vector<std::string> populateTree(const std::string& filter = "") {
	std::vector<std::string> textures;
	addFolderToList("textures", textures, filter);
	return textures;
}

std::string remove_file_extension(const std::string& filename) {
	size_t lastindex = filename.find_last_of(".");
	return filename.substr(0, lastindex);
}


static void ImGuiWritePartOfImage(SDL_Texture* texture, int topx, int topy, int w, int h) {
	int tex_w, tex_h;
	SDL_QueryTexture(texture, nullptr, nullptr, &tex_w, &tex_h);
	float sprite_w = w;
	float sprite_h = h;
	float topxf = topx;
	float topyf = topy;
	ImVec2 uv0 = ImVec2(topxf / tex_w, topyf / tex_h);
	ImVec2 uv1 = ImVec2((topxf + sprite_w) / tex_w, (topyf + sprite_h) / tex_h);
	ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)w, (float)h), uv0, uv1);
}

void SagoTextureSelector::runSpriteSelectorFrame(SDL_Renderer* target) {
	ImGui::Begin("SpriteList", nullptr, ImGuiWindowFlags_NoCollapse);
	static char filter[256] = "";
	ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));
	ImGui::Separator();
	for (const auto& sprite : sprites) {
		std::string sprite_name = sprite.first;
		if (filter[0] == '\0' || sprite_name.find(filter) != std::string::npos) {
			if (ImGui::Selectable(sprite_name.c_str(), selected_sprite == sprite_name)) {
				selected_sprite = sprite_name;
			}
		}
	}

	ImGui::End();

	ImGui::Begin("SpriteViewer");
	if (selected_sprite.length()) {
		const SagoSprite& current_sprite = sprites[selected_sprite];
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(current_sprite.texture);
		ImGui::Text("Size: %d x %d", current_sprite.width, current_sprite.height);
		ImGui::BeginChild("Test");
		int offset = current_sprite.width * (SDL_GetTicks()/current_sprite.frame_time % current_sprite.number_of_frames);
		ImGuiWritePartOfImage(current_texture, current_sprite.topx+offset, current_sprite.topy, current_sprite.width, current_sprite.height);
		ImGui::EndChild();
	}
	ImGui::End();

	ImGui::Begin("SpriteProperties");
	if (selected_sprite.length()) {
		const SagoSprite& current_sprite = sprites[selected_sprite];
		ImGui::Text("Sprite: %s", selected_sprite.c_str());
		ImGui::Separator();
		ImGui::Text("texture: %s", current_sprite.texture.c_str());
		ImGui::Text("topx: %d", current_sprite.topx);
		ImGui::Text("topy: %d", current_sprite.topy);
		ImGui::Text("width: %d", current_sprite.width);
		ImGui::Text("height: %d", current_sprite.height);
		ImGui::Text("number_of_frames: %d", current_sprite.number_of_frames);
		ImGui::Text("frame_time: %d", current_sprite.frame_time);
		ImGui::Text("originx: %d", current_sprite.originx);
		ImGui::Text("originy: %d", current_sprite.originy);
		ImGui::Separator();
		ImGui::Text("meta_name: %s", current_sprite.meta_name.c_str());
		ImGui::Text("meta_from_file: %s", current_sprite.meta_from_file.c_str());
		ImGui::Text("meta_modified: %s", current_sprite.meta_modified.c_str());
	}
	ImGui::End();

	ImGui::Begin("SpriteTexture");
	if (selected_sprite.length() && sprites[selected_sprite].texture.length()) {
		int tex_w, tex_h;
		const SagoSprite& current_sprite = sprites[selected_sprite];
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(current_sprite.texture);
		SDL_QueryTexture(current_texture, nullptr, nullptr, &tex_w, &tex_h);
		ImGui::Text("Size: %d x %d", tex_w, tex_h);
		ImGui::BeginChild("Test");
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)(intptr_t)current_texture, ImVec2((float)tex_w, (float)tex_h));
		ChangeSDLColor color(target);
		color.setYellow();
		for (int i = 1; i < current_sprite.number_of_frames; i++) {
			addRectableToCanvas(target, current_sprite.topx+i*current_sprite.width, current_sprite.topy, current_sprite.height, current_sprite.width,  p.x, p.y);
		}
		color.setRed();
		addRectableToCanvas(target, current_sprite.topx, current_sprite.topy, current_sprite.height, current_sprite.width,  p.x, p.y);
		ImGui::EndChild();
	}
	ImGui::End();
}

void SagoTextureSelector::runTextureSelectorFrame(SDL_Renderer* target) {
	ImGui::Begin("TextureList", nullptr, ImGuiWindowFlags_NoCollapse);
	if (ImGui::SliderInt("grid size", &grid_size, 0, 100))
	{
		Config::getInstance()->setInt(editor_texture_grid_size, grid_size);
	}
	static char filter[256] = "";
	ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));
	ImGui::Separator();
	for (const auto& texture : textures) {
		if (filter[0] == '\0' || texture.find(filter) != std::string::npos) {
			if (ImGui::Selectable(texture.c_str(), selected_texture == texture)) {
				selected_texture = texture;
			}
		}
	}
	ImGui::End();

	ImGui::Begin("TextureViewer");
	if (selected_texture.length()) {
		int tex_w, tex_h;
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(remove_file_extension(selected_texture));
		SDL_QueryTexture(current_texture, nullptr, nullptr, &tex_w, &tex_h);
		ImGui::Text("Size: %d x %d", tex_w, tex_h);
		ImGui::BeginChild("Test");
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)(intptr_t)current_texture, ImVec2((float)tex_w, (float)tex_h));
		ChangeSDLColor color(target);
		color.setRed();
		if (grid_size)
		{
			addLinesToCanvas(target, current_texture, grid_size, grid_size, p.x, p.y);
		}
		ImGui::EndChild();
	}
	ImGui::End();

}



SagoTextureSelector::SagoTextureSelector() {
	grid_size = Config::getInstance()->getInt(editor_texture_grid_size, 32);
}

SagoTextureSelector::~SagoTextureSelector() {
}

bool SagoTextureSelector::IsActive() {
	return isActive;
}

void SagoTextureSelector::ProcessInput(const SDL_Event& event, bool &processed) {
}

void SagoTextureSelector::Draw(SDL_Renderer* target) {
	runTextureSelectorFrame(target);
	runSpriteSelectorFrame(target);
}

void SagoTextureSelector::Update() {
}

void SagoTextureSelector::Init() {
	textures = populateTree();
	sprites = LoadSprites();
}